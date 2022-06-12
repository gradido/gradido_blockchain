#include "gradido_blockchain/model/protobufWrapper/TransactionCreation.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/lib/Decay.h"
#include "Poco/DateTimeFormatter.h"

#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/protobufWrapper/GradidoBlock.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"
#include <sodium.h>


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

			if (!mProtoCreation.has_recipient()) {
				throw TransactionValidationInvalidInputException("not exist", "recipient", "TransferAmount");
			}
			auto recipient_amount = mProtoCreation.recipient();

			auto receiverPublic = recipient_amount.pubkey();
			if (receiverPublic.size() != crypto_sign_PUBLICKEYBYTES) {
				throw TransactionValidationInvalidInputException("invalid size", "pubkey", "public key");
			}

			//
			mMinSignatureCount = 1;
			auto mm = MemoryManager::getInstance();
			auto pubkey_copy = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
			memcpy(*pubkey_copy, receiverPublic.data(), crypto_sign_PUBLICKEYBYTES);
			mForbiddenSignPublicKeys.push_back(pubkey_copy);

			mIsPrepared = true;
			return 0;
		}

		std::string TransactionCreation::getTargetDateString() const
		{
			// proto format is seconds, poco timestamp format is microseconds
			Poco::Timestamp pocoStamp(mProtoCreation.target_date().seconds() * 1000 * 1000);
			//Poco::DateTime(pocoStamp);
			return Poco::DateTimeFormatter::format(pocoStamp, "%d. %b %y");
		}

		Poco::DateTime TransactionCreation::getTargetDate() const
		{
			return DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date());
		}

		bool TransactionCreation::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const GradidoBlock* parentGradidoBlock/* = nullptr*/
		) const
		{
			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				auto mm = MemoryManager::getInstance();
				mpfr_ptr amount = mm->getMathMemory();
				if (mpfr_set_str(amount, mProtoCreation.recipient().amount().data(), 10, gDefaultRound)) {
					throw TransactionValidationInvalidInputException("amount cannot be parsed to a number", "amount", "string");
				}
				if(mpfr_cmp_si(amount, 1000) > 0) {
					throw TransactionValidationInvalidInputException("creation amount to high, max 1000 per month", "amount", "string");
				}
				if (mpfr_cmp_si(amount, 1) < 0) {
					throw TransactionValidationInvalidInputException("creation amount to low, min 1 GDD", "amount", "string");
				}
				mm->releaseMathMemory(amount);
				if (mProtoCreation.recipient().pubkey().size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "recipient pubkey", "public key");
				}
				auto empty = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
				memset(*empty, 0, crypto_sign_PUBLICKEYBYTES);
				if (0 == memcmp(mProtoCreation.recipient().pubkey().data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					throw TransactionValidationInvalidInputException("empty", "recipient pubkey", "public key");
				}
			}

			if (blockchain) {
				if (getCoinGroupId() == blockchain->getGroupId()) {
					throw TransactionValidationInvalidInputException(
						"coinGroupId shouldn't be set if it is the same as blockchain group alias",
						"coinGroupId", "string or UUID"
					);
				}
			}

			if ((level & TRANSACTION_VALIDATION_DATE_RANGE) == TRANSACTION_VALIDATION_DATE_RANGE)
			{
				Poco::DateTime targetDate = Poco::Timestamp(mProtoCreation.target_date().seconds() * Poco::Timestamp::resolution());
				assert(blockchain);
				assert(parentGradidoBlock);

				auto pubkey = mProtoCreation.recipient().pubkey();
				auto mm = MemoryManager::getInstance();
				auto amount = mm->getMathMemory();
				if (mpfr_set_str(amount, mProtoCreation.recipient().amount().data(), 10, gDefaultRound)) {
					throw TransactionValidationInvalidInputException("amount cannot be parsed to a number", "amount", "string");
				}
				mpfr_ptr sum;
				auto received = parentGradidoBlock->getReceivedAsTimestamp();
				auto creationMaxAlgo = getCorrectCreationMaxAlgo(received);
				auto targetCreationMaxAlgo = getCorrectCreationMaxAlgo(targetDate);

				if (CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo) {
					sum = calculateCreationSumLegacy(pubkey, received, blockchain);
				}
				else if(CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo) {
					sum = calculateCreationSum(pubkey, targetDate.month(), targetDate.year(), parentGradidoBlock->getReceivedAsTimestamp(), blockchain);
				}
				mpfr_add(sum, sum, amount, gDefaultRound);

				auto id = parentGradidoBlock->getID();
				int lastId = 0;
				auto lastTransaction = blockchain->getLastTransaction();
				if (!lastTransaction.isNull()) {
					lastId = lastTransaction->getID();
				}
				if (id <= lastId) {
					// this transaction was already added to blockchain and therefor also added in calculateCreationSum
					mpfr_sub(sum, sum, amount, gDefaultRound);
				}
				// first max creation check algo
				if (CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo && mpfr_cmp_si(sum, 3000) > 0) {
					mpfr_sub(sum, sum, amount, gDefaultRound);
					std::string alreadyCreatedSum;
					TransactionBase::amountToString(&alreadyCreatedSum, sum);
					throw InvalidCreationException(
						"creation more than 3.000 GDD in 3 month not allowed",
						targetDate.month(), targetDate.year(),
						mProtoCreation.recipient().amount(),
						alreadyCreatedSum
					);
				}
				// first and second max creation check algo together (transitional phase)
				/*else if (CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo &&
						 CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == targetCreationMaxAlgo) {
					auto sum2 = calculateCreationSumLegacy(pubkey, targetDate, blockchain);
					std::string temp;
					TransactionBase::amountToString(&temp, sum2);
					//printf("")
					mm->releaseMathMemory(sum2);

				}*/
				// second max creation check algo
				else if (CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo && mpfr_cmp_si(sum, 1000) > 0) {
					mpfr_sub(sum, sum, amount, gDefaultRound);
					std::string alreadyCreatedSum;
					TransactionBase::amountToString(&alreadyCreatedSum, sum);
					throw InvalidCreationException(
						"creation more than 1.000 GDD per month not allowed",
						targetDate.month(), targetDate.year(),
						mProtoCreation.recipient().amount(),
						alreadyCreatedSum
					);
				}

				// TODO: replace with variable, state transaction for group
				/*if (CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo && mpfr_cmp_si(sum, 1000) > 0 ||
					CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo && mpfr_cmp_si(sum, 3000) > 0) {
					//throw TransactionValidationInvalidInputException("creation more than 1.000 GDD per month not allowed", "amount");
					mpfr_sub(sum, sum, amount, gDefaultRound);
					std::string alreadyCreatedSum;
					TransactionBase::amountToString(&alreadyCreatedSum, sum);
					std::string errorMessage = "creation more than 1.000 GDD per month not allowed";
					if (CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo) {
						errorMessage = "creation more than 3.000 GDD in 3 month not allowed";
					}
					throw InvalidCreationException(
						errorMessage.data(),
						targetDate.month(), targetDate.year(),
						mProtoCreation.recipient().amount(),
						alreadyCreatedSum
					);
				}*/
				mm->releaseMathMemory(sum);
				mm->releaseMathMemory(amount);
			}
			// TODO: check if signer belongs to blockchain
			if ((level & TRANSACTION_VALIDATION_CONNECTED_GROUP) == TRANSACTION_VALIDATION_CONNECTED_GROUP) {
				assert(blockchain);
				auto addressType = blockchain->getAddressType(getRecipientPublicKeyString());
				if (addressType != proto::gradido::RegisterAddress_AddressType_HUMAN) {
					throw WrongAddressTypeException("wrong address type for creation", addressType);
				}
			}

			return true;
		}

		bool TransactionCreation::validateTargetDate(uint64_t receivedSeconds) const
		{
			auto target_date = Poco::DateTime(DataTypeConverter::convertFromProtoTimestampSeconds(mProtoCreation.target_date()));
			auto received = Poco::DateTime(Poco::Timestamp(receivedSeconds * Poco::Timestamp::resolution()));

			auto targetDateReceivedDistanceMonth = getTargetDateReceivedDistanceMonth(received);
			//  2021-09-01 02:00:00 | 2021-12-04 01:22:14
			if (target_date.year() == received.year())
			{
				if (target_date.month() + targetDateReceivedDistanceMonth < received.month()) {
					std::string errorMessage =
						"year is the same, target date month is more than "
						+ std::to_string(targetDateReceivedDistanceMonth)
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
			else if (target_date.year() + 1 < received.year())
			{
				throw TransactionValidationInvalidInputException("target date year is in past", "target_date", "date time");
			}
			else
			{
				// target_date.year +1 == now.year
				if (target_date.month() + targetDateReceivedDistanceMonth < received.month() + 12) {
					std::string errorMessage =
						"target date month is more than "
						+ std::to_string(targetDateReceivedDistanceMonth)
						+ " month in past";
					throw TransactionValidationInvalidInputException(errorMessage.data(), "target_date", "date time");
				}
			}
			return true;
		}

		mpfr_ptr TransactionCreation::calculateCreationSum(
			const std::string& address,
			int month,
			int year,
			Poco::DateTime received,
			IGradidoBlockchain* blockchain
		)
		{
			assert(blockchain);
			std::vector<Poco::SharedPtr<model::TransactionEntry>> allTransactions;
			// received = max
			// received - 2 month = min
			auto monthDiff = model::gradido::TransactionCreation::getTargetDateReceivedDistanceMonth(received);
			Poco::DateTime searchDate = received;
			auto mm = MemoryManager::getInstance();
			for (int i = 0; i < monthDiff + 1; i++) {
				auto transactions = blockchain->findTransactions(address, searchDate.month(), searchDate.year());
				// https://stackoverflow.com/questions/201718/concatenating-two-stdvectors
				allTransactions.insert(
					allTransactions.end(),
					std::make_move_iterator(transactions.begin()),
					std::make_move_iterator(transactions.end())
				);
				searchDate -= Poco::Timespan(Poco::DateTime::daysOfMonth(searchDate.year(), searchDate.month()), 0, 0, 0, 0);
			}
			//printf("[Group::calculateCreationSum] from group: %s\n", mGroupAlias.data());
			auto amount = mm->getMathMemory();
			auto sum = mm->getMathMemory();
			for (auto it = allTransactions.begin(); it != allTransactions.end(); it++) {
				auto gradidoBlock = std::make_unique<model::gradido::GradidoBlock>((*it)->getSerializedTransaction());
				auto body = gradidoBlock->getGradidoTransaction()->getTransactionBody();
				if (body->getTransactionType() == model::gradido::TRANSACTION_CREATION) {
					auto creation = body->getCreationTransaction();
					auto targetDate = creation->getTargetDate();
					if (targetDate.month() != month || targetDate.year() != year) {
						continue;
					}
					//printf("added from transaction: %d \n", gradidoBlock->getID());
					mpfr_set_str(amount, creation->getAmount().data(), 10, gDefaultRound);
					mpfr_add(sum, sum, amount, gDefaultRound);
				}
			}
			mm->releaseMathMemory(amount);
			return sum;
		}

		mpfr_ptr TransactionCreation::calculateCreationSumLegacy(
			const std::string& address,
			Poco::DateTime received,
			IGradidoBlockchain* blockchain
		)
		{
			assert(blockchain);
			// check that is is indeed an old transaction from before Sun May 03 2020 11:00:08 GMT+0000
			auto algo = getCorrectCreationMaxAlgo(received);
			assert(CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == algo);
			std::vector<Poco::SharedPtr<model::TransactionEntry>> allTransactions;
			// received = max
			// received - 2 month = min
			//auto monthDiff = model::gradido::TransactionCreation::getTargetDateReceivedDistanceMonth(received);
			int monthDiff = 2;
			Poco::DateTime searchDate = received;
			auto mm = MemoryManager::getInstance();
			for (int i = 0; i < monthDiff + 1; i++) {
				auto transactions = blockchain->findTransactions(address, searchDate.month(), searchDate.year());
				// https://stackoverflow.com/questions/201718/concatenating-two-stdvectors
				allTransactions.insert(
					allTransactions.end(),
					std::make_move_iterator(transactions.begin()),
					std::make_move_iterator(transactions.end())
				);
				searchDate -= Poco::Timespan(Poco::DateTime::daysOfMonth(searchDate.year(), searchDate.month()), 0, 0, 0, 0);
			}
			//printf("[Group::calculateCreationSum] from group: %s\n", mGroupAlias.data());
			auto amount = mm->getMathMemory();
			auto sum = mm->getMathMemory();
			for (auto it = allTransactions.begin(); it != allTransactions.end(); it++) {
				auto gradidoBlock = std::make_unique<model::gradido::GradidoBlock>((*it)->getSerializedTransaction());
				auto body = gradidoBlock->getGradidoTransaction()->getTransactionBody();
				if (body->getTransactionType() == model::gradido::TRANSACTION_CREATION) {
					auto creation = body->getCreationTransaction();
					//printf("added from transaction: %d \n", gradidoBlock->getID());
					mpfr_set_str(amount, creation->getAmount().data(), 10, gDefaultRound);
					mpfr_add(sum, sum, amount, gDefaultRound);
				}
			}
			mm->releaseMathMemory(amount);
			return sum;
		}

		std::vector<MemoryBin*> TransactionCreation::getInvolvedAddresses() const
		{
			auto mm = MemoryManager::getInstance();
			auto recipientPubkeySize = mProtoCreation.recipient().pubkey().size();
			auto recipientPubkey = mm->getMemory(recipientPubkeySize);
			memcpy(*recipientPubkey, mProtoCreation.recipient().pubkey().data(), recipientPubkeySize);
			return { recipientPubkey };
		}
		bool TransactionCreation::isInvolved(const std::string pubkeyString) const
		{
			return mProtoCreation.recipient().pubkey() == pubkeyString;
		}

		const std::string& TransactionCreation::getCoinGroupId() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().group_id();
		}

		bool TransactionCreation::isBelongToUs(const TransactionBase* pairingTransaction) const
		{
			auto pair = dynamic_cast<const TransactionCreation*>(pairingTransaction);
			bool belongToUs = true;
			auto mm = MemoryManager::getInstance();
			if (getTargetDate() != pair->getTargetDate()) {
				belongToUs = false;
			}
			if (getAmount() != pair->getAmount()) {
				belongToUs = false;
			}

			if (getCoinGroupId() != pair->getCoinGroupId()) {
				belongToUs = false;
			}

			auto recipient = getRecipientPublicKey();
			auto pairRecipient = pair->getRecipientPublicKey();
			if (!recipient->isSame(pairRecipient)) {
				belongToUs = false;
			}
			mm->releaseMemory(recipient); mm->releaseMemory(pairRecipient);
			return belongToUs;
		}

		const std::string& TransactionCreation::getAmount() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().amount();
		}


		MemoryBin* TransactionCreation::getRecipientPublicKey() const
		{
			auto bin = MemoryManager::getInstance()->getMemory(mProtoCreation.recipient().pubkey().size());
			bin->copyFromProtoBytes(mProtoCreation.recipient().pubkey());
			return bin;
		}

		const std::string& TransactionCreation::getRecipientPublicKeyString() const
		{
			// cannot inline, because this doens't work in dll build
			return mProtoCreation.recipient().pubkey();
		}

		std::string TransactionCreation::toDebugString() const
		{
			std::string result;
			auto targetDateString = Poco::DateTimeFormatter::format(getTargetDate(), Poco::DateTimeFormat::SORTABLE_FORMAT);
			result += "creation: " + getAmount() + " GDD, target: " + targetDateString + "\n";
			result += "to:   " + DataTypeConverter::binToHex(getRecipientPublicKeyString()) + "\n";
			return std::move(result);
		}

		int TransactionCreation::getTargetDateReceivedDistanceMonth(Poco::DateTime received)
		{
			int targetDateReceivedDistanceMonth = 2;
			// extra rule from the beginning and testing phase to keep transactions from beginning valid
			// allow 3 month distance between created and target date between this dates
			// 1585551913 = Mon Mar 30 2020 07:05:13 GMT+0000
			// 1641681224 = Sat Jan 08 2022 22:33:44 GMT+0000
			if (received.timestamp() > 1585551915 * Poco::Timestamp::resolution() && received.timestamp() < 1641681224 * Poco::Timestamp::resolution()) {
				targetDateReceivedDistanceMonth = 3;
			}
			return targetDateReceivedDistanceMonth;
		}

		TransactionCreation::CreationMaxAlgoVersion TransactionCreation::getCorrectCreationMaxAlgo(Poco::DateTime date)
		{
			if (date.timestamp() < 1588503608 * Poco::Timestamp::resolution()) {
				return CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD;
			}
			return CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE;
		}

	}
}

