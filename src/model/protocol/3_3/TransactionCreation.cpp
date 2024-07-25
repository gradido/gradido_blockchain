#include "date/date.h"

#include "gradido_blockchain/model/protobufWrapper/TransactionCreation.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/Decay.h"
#include "gradido_blockchain/lib/Decimal.h"

#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/ConfirmedTransaction.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"

#include <sodium.h>

using namespace std::chrono;

namespace model {
	namespace gradido {

		TransactionCreation::TransactionCreation(const proto::gradido::GradidoCreation& protoCreation)
			: mProtoCreation(protoCreation)
		{
		}

		TransactionCreation::~TransactionCreation()
		{

		}

		int TransactionCreation::prepare()
		{
			if (mIsPrepared) return 0;
			mMinSignatureCount = 1;
			mForbiddenSignPublicKeys.push_back(mProtoCreation.recipient().pubkey());
			mIsPrepared = true;
			return 0;
		}

		Timepoint TransactionCreation::getTargetDate() const
		{
			return DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date());
		}

		bool TransactionCreation::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const ConfirmedTransaction* parentConfirmedTransaction/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				
				if (!mProtoCreation.has_recipient()) {
					throw TransactionValidationInvalidInputException("not exist", "recipient", "TransferAmount");
				}
				Decimal amount(mProtoCreation.recipient().amount());
				if(amount > 1000) {
					throw TransactionValidationInvalidInputException("creation amount to high, max 1000 per month", "amount", "string");
				}
				if (amount < 0) {
					throw TransactionValidationInvalidInputException("creation amount to low, min 1 GDD", "amount", "string");
				}
				validate25519PublicKey(mProtoCreation.recipient().pubkey(), "recipient pubkey");
			}

			if (blockchain) {
				if (getCoinCommunityId() == blockchain->getGroupId()) {
					throw TransactionValidationInvalidInputException(
						"coinGroupId shouldn't be set if it is the same as blockchain group alias",
						"coinGroupId", "string or UUID"
					);
				}
			}

			if ((level & TRANSACTION_VALIDATION_DATE_RANGE) == TRANSACTION_VALIDATION_DATE_RANGE)
			{
				assert(blockchain);
				assert(parentConfirmedTransaction);

				auto pubkey = mProtoCreation.recipient().pubkey();
				Decimal amount(mProtoCreation.recipient().amount());
				Decimal sum;
				auto confirmedAt = parentConfirmedTransaction->getConfirmedAtAsTimepoint();
				auto creationMaxAlgo = getCorrectCreationMaxAlgo(confirmedAt);
				auto targetDate = DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date());
				auto ymd = date::year_month_day{ date::floor<date::days>(targetDate) };
				auto targetCreationMaxAlgo = getCorrectCreationMaxAlgo(targetDate);

				if (CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo) {
					sum = calculateCreationSumLegacy(pubkey, confirmedAt, blockchain);
				}
				else if(CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo) {
					sum = calculateCreationSum(
						pubkey, 
						static_cast<uint32_t>(ymd.month()),
						static_cast<int>(ymd.year()), 
						parentConfirmedTransaction->getConfirmedAtAsTimepoint(),
						blockchain
					);
				}
				sum += amount;
				auto id = parentConfirmedTransaction->getID();
				int lastId = 0;
				auto lastTransaction = blockchain->getLastTransaction();
				if (lastTransaction) {
					lastId = lastTransaction->getID();
				}
				if (id <= lastId) {
					// this transaction was already added to blockchain and therefor also added in calculateCreationSum
					sum -= amount;
				}
				// first max creation check algo
				if (CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo && sum > 3000) {
					sum -= amount;
					throw InvalidCreationException(
						"creation more than 3.000 GDD in 3 month not allowed",
						static_cast<uint32_t>(ymd.month()), static_cast<int>(ymd.year()),
						mProtoCreation.recipient().amount(),
						sum.toString()
					);
				}
				// second max creation check algo
				else if (CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo && sum > 1000) {
					sum -= amount;
					throw InvalidCreationException(
						"creation more than 1.000 GDD per month not allowed",
						static_cast<uint32_t>(ymd.month()), static_cast<int>(ymd.year()),
						mProtoCreation.recipient().amount(),
						sum.toString()
					);
				}
			}
			// TODO: check if signer belongs to blockchain
			if ((level & TRANSACTION_VALIDATION_CONNECTED_GROUP) == TRANSACTION_VALIDATION_CONNECTED_GROUP) {
				assert(blockchain);
				auto addressType = blockchain->getAddressType(getRecipientPublicKey());
				if (addressType != proto::gradido::RegisterAddress_AddressType_COMMUNITY_HUMAN &&
					addressType != proto::gradido::RegisterAddress_AddressType_COMMUNITY_AUF && 
					addressType != proto::gradido::RegisterAddress_AddressType_COMMUNITY_GMW) {
					throw WrongAddressTypeException("wrong address type for creation", addressType, getRecipientPublicKey());
				}
			}

			return true;
		}

		bool TransactionCreation::validateTargetDate(uint64_t receivedSeconds) const
		{
			auto target_date = date::year_month_day{ date::floor<date::days>(DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date()))};
			Timepoint receivedTimePoint{ std::chrono::seconds(receivedSeconds) };
			auto received = date::year_month_day{ date::floor<date::days>(receivedTimePoint) };

			auto targetDateReceivedDistanceMonth = getTargetDateReceivedDistanceMonth(receivedTimePoint);
			//  2021-09-01 02:00:00 | 2021-12-04 01:22:14
			if (target_date.year() == received.year())
			{
				if (static_cast<unsigned>(target_date.month()) + targetDateReceivedDistanceMonth < static_cast<unsigned>(received.month())) {
					std::string errorMessage =
						"year is the same, target date month is more than "
						+ std::to_string(static_cast<unsigned>(targetDateReceivedDistanceMonth))
						+ " month in past";
					throw TransactionValidationInvalidInputException(errorMessage.data(), "target_date", "date time");
				}
				if (target_date.month() > received.month()) {
					throw TransactionValidationInvalidInputException("year is the same, target date month is in future", "target_date", "date time");
				}
			}
			else if (target_date.year() > received.year())
			{
				throw TransactionValidationInvalidInputException("target date year is in future", "target_date", "date time");
			}
			else if (static_cast<int>(target_date.year()) + 1 < static_cast<int>(received.year()))
			{
				throw TransactionValidationInvalidInputException("target date year is in past", "target_date", "date time");
			}
			else
			{
				// target_date.year +1 == now.year
				if (static_cast<unsigned>(target_date.month()) + targetDateReceivedDistanceMonth < static_cast<unsigned>(received.month()) + 12) {
					std::string errorMessage =
						"target date month is more than "
						+ std::to_string(static_cast<unsigned>(targetDateReceivedDistanceMonth))
						+ " month in past";
					throw TransactionValidationInvalidInputException(errorMessage.data(), "target_date", "date time");
				}
			}
			return true;
		}

		Decimal TransactionCreation::calculateCreationSum(
			const std::string& address,
			int month,
			int year,
			Timepoint received,
			IGradidoBlockchain* blockchain
		)
		{
			assert(blockchain);
			std::vector<std::shared_ptr<model::TransactionEntry>> allTransactions;
			// received = max
			// received - 2 month = min
			auto monthDiff = model::gradido::TransactionCreation::getTargetDateReceivedDistanceMonth(received);
			auto searchDate = date::year_month_day{ date::floor<date::days>(received) };
			for (int i = 0; i < monthDiff + 1; i++) 
			{	
				auto transactions = blockchain->findTransactions(address, static_cast<unsigned>(searchDate.month()), static_cast<int>(searchDate.year()));
				if(transactions.size()) {
				// https://stackoverflow.com/questions/201718/concatenating-two-stdvectors
					allTransactions.insert(
						allTransactions.end(),
						std::make_move_iterator(transactions.begin()),
						std::make_move_iterator(transactions.end())
					);
				}
				searchDate.month()--;
			}
			//printf("[Group::calculateCreationSum] from group: %s\n", mGroupAlias.data());
			Decimal amount, sum;
			for (auto it = allTransactions.begin(); it != allTransactions.end(); it++) 
			{
				auto confirmedTransaction = std::make_unique<model::gradido::ConfirmedTransaction>((*it)->getSerializedTransaction());
				auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
				if (body->getTransactionType() == model::gradido::TRANSACTION_CREATION) {
					auto creation = body->getCreationTransaction();
					auto targetDate = date::year_month_day{ date::floor<date::days>(creation->getTargetDate()) };
					if (targetDate.month() != date::month(month) || targetDate.year() != date::year(year)) {
						continue;
					}
					//printf("added from transaction: %d \n", gradidoBlock->getID());
					amount.fromString(creation->getAmount());
					sum += amount;
				}
			}
			return sum;
		}

		Decimal TransactionCreation::calculateCreationSumLegacy(
			const std::string& address,
			Timepoint received,
			IGradidoBlockchain* blockchain
		)
		{
			assert(blockchain);
			// check that is is indeed an old transaction from before Sun May 03 2020 11:00:08 GMT+0000
			auto algo = getCorrectCreationMaxAlgo(received);
			assert(CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == algo);
			std::vector<std::shared_ptr<model::TransactionEntry>> allTransactions;
			// received = max
			// received - 2 month = min
			//auto monthDiff = model::gradido::TransactionCreation::getTargetDateReceivedDistanceMonth(received);
			int monthDiff = 2;
			auto searchDate = date::year_month_day{ date::floor<date::days>(received) };
			for (int i = 0; i < monthDiff + 1; i++) {
				auto transactions = blockchain->findTransactions(address, static_cast<unsigned>(searchDate.month()), static_cast<int>(searchDate.year()));
				// https://stackoverflow.com/questions/201718/concatenating-two-stdvectors
				allTransactions.insert(
					allTransactions.end(),
					std::make_move_iterator(transactions.begin()),
					std::make_move_iterator(transactions.end())
				);
				searchDate.month()--;
			}
			//printf("[Group::calculateCreationSum] from group: %s\n", mGroupAlias.data());
			Decimal amount, sum;
			for (auto it = allTransactions.begin(); it != allTransactions.end(); it++) {
				auto confirmedTransaction = std::make_unique<model::gradido::ConfirmedTransaction>((*it)->getSerializedTransaction());
				auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
				if (body->getTransactionType() == model::gradido::TRANSACTION_CREATION) {
					auto creation = body->getCreationTransaction();
					//printf("added from transaction: %d \n", gradidoBlock->getID());
					mpfr_set_str(amount, creation->getAmount().data(), 10, gDefaultRound);
					mpfr_add(sum, sum, amount, gDefaultRound);
				}
			}
			return sum;
		}

		std::vector<std::string_view> TransactionCreation::getInvolvedAddresses() const
		{
			return { mProtoCreation.recipient().pubkey() };
		}
		bool TransactionCreation::isInvolved(const std::string& pubkeyString) const
		{
			return mProtoCreation.recipient().pubkey() == pubkeyString;
		}

		const std::string& TransactionCreation::getCoinCommunityId() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().community_id();
		}

		bool TransactionCreation::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const TransactionCreation*>(pairingTransaction);
			if (getTargetDate() != pair->getTargetDate() ||
				getAmount() != pair->getAmount() ||
				getCoinCommunityId() != pair->getCoinCommunityId() ||
				getRecipientPublicKey() != pair->getRecipientPublicKey()) {
				return false;
			}
			return true;
		}

		const std::string& TransactionCreation::getAmount() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().amount();
		}

		const std::string& TransactionCreation::getRecipientPublicKey() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().pubkey();
		}

		std::string TransactionCreation::toDebugString() const
		{
			std::string result;
			auto targetDateString = DataTypeConverter::timePointToString(getTargetDate());
			result += "creation: " + getAmount() + " GDD, target: " + targetDateString + "\n";
			result += "to:   " + DataTypeConverter::binToHex(getRecipientPublicKey()) + "\n";
			return std::move(result);
		}

		unsigned TransactionCreation::getTargetDateReceivedDistanceMonth(Timepoint received)
		{
			date::month targetDateReceivedDistanceMonth(2);
			// extra rule from the beginning and testing phase to keep transactions from beginning valid
			// allow 3 month distance between created and target date between this dates
			// 1585544394 = Mon Mar 30 2020 04:59:54 GMT+0000
			// 1641681224 = Sat Jan 08 2022 22:33:44 GMT+0000
			auto secondsSinceEpoch = time_point_cast<std::chrono::seconds>(received).time_since_epoch().count();
			if (secondsSinceEpoch > 1585544394 && secondsSinceEpoch < 1641681224) {
				targetDateReceivedDistanceMonth = date::month(3);
			}
			return static_cast<unsigned>(targetDateReceivedDistanceMonth);
		}

		TransactionCreation::CreationMaxAlgoVersion TransactionCreation::getCorrectCreationMaxAlgo(Timepoint date)
		{
			auto secondsSinceEpoch = time_point_cast<std::chrono::seconds>(date).time_since_epoch().count();
			if (secondsSinceEpoch < 1588503608) {
				return CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD;
			}
			return CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE;
		}

	}
}

