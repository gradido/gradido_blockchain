#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTxCold.h"
#include "gradido_blockchain/data/compact/CreationTx.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/GradidoCreation.h"
#include "gradido_blockchain/interaction/calculateCreationSum/Context.h"
#include "gradido_blockchain/interaction/validate/Error.h"
#include "gradido_blockchain/interaction/validate/ErrorType.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/validate/Options.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"

#include <memory>
#include <string>

using std::shared_ptr;
using std::string, std::to_string;
using DataTypeConverter::timePointToString;

namespace gradido {
	using blockchain::CompactFilter, blockchain::Filter;
	using data::adapter::toPublicKeyIndex;
	using data::AddressType, data::SignatureMap, data::GradidoCreation, data::ConfirmedTransaction;
	using data::compact::ConfirmedGradidoTx, data::compact::ConstConfirmedTxPtr, data::compact::ConfirmedGradidoTxCold, data::compact::CreationTx;

	namespace interaction::validate {

		GradidoCreationRole::GradidoCreationRole(shared_ptr<const GradidoCreation> gradidoCreation)
			: mGradidoCreation(gradidoCreation)
		{
			assert(gradidoCreation);
			// prepare for signature check
			mMinSignatureCount = 1;
			mForbiddenSignPublicKeys.push_back(mGradidoCreation->getRecipient().getPublicKey());
		}

		void GradidoCreationRole::run(Type type, ContextData& c)
		{
			const auto& recipient = mGradidoCreation->getRecipient();
			TransferAmountRole transferAmountRole(mGradidoCreation->getRecipient());
			transferAmountRole.run(type, c);

			if ((type & Type::SINGLE) == Type::SINGLE)
			{
				validateEd25519PublicKey(recipient.getPublicKey(), "recipient pubkey");
				auto recipientAmount = recipient.getAmount();
				if (recipientAmount > GradidoUnit(1000.0)) {
					throw TransactionValidationInvalidInputException(
						"creation amount to high, max 1000 per month",
						"amount",
						"string",
						"<= 10000",
						recipientAmount.toString().data()
					);
				}
				if (recipientAmount < GradidoUnit::fromGradidoCent(2000)) {
					throw TransactionValidationInvalidInputException(
						"creation amount to low, min 0.2 GDD",
						"amount",
						"string",
						">= 0.2",
						recipientAmount.toString().data()
					);
				}
			}

			if ((type & Type::MONTH_RANGE) == Type::MONTH_RANGE)
			{
				if (!c.senderPreviousConfirmedTransaction) {
					throw GradidoNullPointerException(
						"missing previous confirmed transaction for interaction::validate Creation",
						"data::ConstConfirmedTransactionPtr",
						__FUNCTION__
					);
				}
				assert(mConfirmedAt.getSeconds());
				auto publicKeyIndex = toPublicKeyIndex( mGradidoCreation->getRecipient().getPublicKey(), c.recipientBlockchain->getCommunityIdIndex());
				calculateCreationSum::Context calculateCreationSum(
					mConfirmedAt,
					mGradidoCreation->getTargetDate(),
					publicKeyIndex,
					c.senderPreviousConfirmedTransaction->getId()
				);

				GradidoUnit sum = calculateCreationSum.run(*c.senderBlockchain);
				sum += recipient.getAmount();
				if (sum > calculateCreationSum.getLimit()) {
					auto targetDate = mGradidoCreation->getTargetDate();
					auto ymd = date::year_month_day{ date::floor<date::days>(targetDate.getAsTimepoint()) };
					sum -= recipient.getAmount();
					string message = "creation more than ";
					message += calculateCreationSum.getLimit().toString() + " not allowed";

					throw InvalidCreationException(
						message.data(),
						static_cast<uint32_t>(ymd.month()), static_cast<int>(ymd.year()),
						recipient.getAmount(), sum
					);
				}
			}
			if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
				Filter filter(Filter::LAST_TRANSACTION);
				filter.involvedPublicKey = mGradidoCreation->getRecipient().getPublicKey();
				auto addressType = c.senderBlockchain->getAddressType(filter);
				if (AddressType::COMMUNITY_HUMAN != addressType) {
					throw WrongAddressTypeException(
						"wrong address type for creation",
						addressType,
						mGradidoCreation->getRecipient().getPublicKey(),
						c.senderBlockchain->getCommunityIdIndex()
					);
				}
			}
		}

		void GradidoCreationRole::checkRequiredSignatures(
			const SignatureMap& signatureMap,
			shared_ptr<blockchain::Abstract> blockchain /*  = nullptr*/
		) const
		{
			AbstractRole::checkRequiredSignatures(signatureMap, blockchain);
			if (!blockchain) return;
			auto& signPairs = signatureMap.getSignaturePairs();
			// check for account type
			for (auto& signPair : signPairs) {
				Filter filter(Filter::LAST_TRANSACTION);
				filter.involvedPublicKey = signPair.getPublicKey();
				filter.timepointInterval = TimepointInterval(blockchain->getStartDate(), mCreatedAt);

				auto signerAccountType = blockchain->getAddressType(filter);
				if (AddressType::COMMUNITY_HUMAN != signerAccountType) {
					throw WrongAddressTypeException(
						"signer for creation doesn't have a community human account",
						signerAccountType,
						signPair.getPublicKey(),
						blockchain->getCommunityIdIndex()
					);
				}
			}
		}

		void GradidoCreationRole::validateTargetDate(Timepoint createdAtTimePoint)
		{
			auto target_date = date::year_month_day{ date::floor<date::days>(mGradidoCreation->getTargetDate().getAsTimepoint())};
			auto received = date::year_month_day{ date::floor<date::days>(createdAtTimePoint) };

			auto targetDateReceivedDistanceMonth = getTargetDateReceivedDistanceMonth(createdAtTimePoint);
			//  2021-09-01 02:00:00 | 2021-12-04 01:22:14
			if (target_date.year() == received.year())
			{
				if (static_cast<unsigned>(target_date.month()) + targetDateReceivedDistanceMonth < static_cast<unsigned>(received.month())) {
					std::string expected = ">= "
						+ timePointToString(createdAtTimePoint)
						+ " - "
						+ to_string(static_cast<unsigned>(targetDateReceivedDistanceMonth))
						+ " months"
					;
					throw TransactionValidationInvalidInputException(
						"year is the same, target date month is invalid",
						"target_date",
						"TimestampSeconds",
						expected.data(),
						timePointToString(mGradidoCreation->getTargetDate().getAsTimepoint()).data()
					);
				}
				if (target_date.month() > received.month()) {
					string expected = "<= " + to_string(static_cast<unsigned>(received.month()));
					throw TransactionValidationInvalidInputException(
						"year is the same, target date month is invalid",
						"target_date",
						"TimestampSeconds",
						expected.data(),
						to_string(static_cast<unsigned>(target_date.month())).data()
					);
				}
			}
			else if (target_date.year() > received.year())
			{
				string expected = "<= " + to_string(static_cast<int>(received.year()));
				throw TransactionValidationInvalidInputException(
					"target date year is in future",
					"target_date",
					"TimestampSeconds",
					expected.data(),
					to_string(static_cast<int>(target_date.year())).data()
				);
			}
			else if (static_cast<int>(target_date.year()) + 1 < static_cast<int>(received.year()))
			{
				string expected = " >= " + to_string(static_cast<int>(received.year())) + " - 1 year";
				throw TransactionValidationInvalidInputException(
					"target date year is in past",
					"target_date",
					"TimestampSeconds",
					expected.data(),
					to_string(static_cast<int>(target_date.year())).data()
				);
			}
			else
			{
				// target_date.year +1 == now.year
				if (static_cast<unsigned>(target_date.month()) + targetDateReceivedDistanceMonth < static_cast<unsigned>(received.month()) + 12) {
					string expected = ">= "
						+ timePointToString(createdAtTimePoint)
						+ " - "
						+ to_string(static_cast<unsigned>(targetDateReceivedDistanceMonth))
						+ " months"
					;
					throw TransactionValidationInvalidInputException(
						"target date month is invalid",
						"target_date",
						"TimestampSeconds",
						expected.data(),
						timePointToString(mGradidoCreation->getTargetDate().getAsTimepoint()).data()
					);
				}
			}
		}

		unsigned GradidoCreationRole::getTargetDateReceivedDistanceMonth(Timepoint createdAt)
		{
			date::month targetDateReceivedDistanceMonth(2);
			// extra rule from the beginning and testing phase to keep transactions from beginning valid
			// allow 3 month distance between created and target date between this dates
			// 1585544394 = Mon Mar 30 2020 04:59:54 GMT+0000
			// 1641681224 = Sat Jan 08 2022 22:33:44 GMT+0000
			auto secondsSinceEpoch = time_point_cast<std::chrono::seconds>(createdAt).time_since_epoch().count();
			if (secondsSinceEpoch > 1585544394 && secondsSinceEpoch < 1641681224) {
				targetDateReceivedDistanceMonth = date::month(3);
			}
			return static_cast<unsigned>(targetDateReceivedDistanceMonth);
		}

		static Error validateMonthRange(const ConfirmedGradidoTx& tx, const blockchain::Abstract& blockchain)
		{
			const auto& creation = tx.specific.creation;
			ConstConfirmedTxPtr previousTx;
			if (tx.isConfirmedTx()) {
				previousTx = blockchain.getConfirmedTxForId(tx.txNr - 1);
			}
			else {
				previousTx = blockchain.findOne(CompactFilter::lastTransaction());
			}
			if (!previousTx) {
				throw GradidoNullPointerException(
					"missing previous confirmed transaction for interaction::validate Creation",
					"data::ConstConfirmedTransactionPtr",
					__FUNCTION__
				);
			}
			auto confirmedAt = tx.getConfirmedAt();
			auto amount = tx.getAmount();
			auto limit = calculateCreationSum::getLimit(confirmedAt);

			GradidoUnit previousContributionsSum = calculateCreationSum::calculateCreationSum(
				confirmedAt,
				monthYearToTimepoint(creation.targetMonthYear),
				tx.getRecipient(),
				blockchain,
				previousTx->txNr
			);
			auto sumAfterContribution = previousContributionsSum + amount;
			
			if (sumAfterContribution > limit) {
				string message = "creation more than ";
				message += limit.toString() + " not allowed";
				return {
					.type = ErrorType::Contribution_Sum_Exceeded,
					.message = message,
					.actual = amount.toString(),
					.expected = "<= " + (limit - previousContributionsSum).toString()
				};
			}
			return { .type = ErrorType::Success };
		}

		static Error validateSignatures(
			const CreationTx& creation,
			const ConfirmedGradidoTxCold* coldData,
			const IDictionary<PublicKey>& publicKeyDictionary
		) {
			if (coldData->signatureMap.size() != 1) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "unexpected signature count",
					.actual = to_string(coldData->signatureMap.size()),
					.expected = "1"
				};
			}
			auto signerPublicKeyIndex = publicKeyDictionary.getIndexForData(coldData->signatureMap[0].first);
			if (signerPublicKeyIndex == creation.recipientPublicKeyIndex) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "creation must be signed from other public key as the recipient"
				};
			}
			return { .type = ErrorType::Success };
		}

		static Error validateAccountBalancesSingle(const ConfirmedGradidoTx& tx)
		{
			const auto& creation = tx.specific.creation;
			if (tx.accountBalanceCount != 3) {
				return { .type = ErrorType::Invalid_Field, .message = "unexpected account balances, expect recipient, gmw and auf on creation" };
			}
			bool creationAccountBalanceFound = false;
			for (int i = 0; i < tx.accountBalanceCount; i++)
			{
				const auto& accountBalance = tx.accountBalances[i];
				if (accountBalance.balanceGddCent < creation.amountGddCent) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Creation account balance couldn't be smaller than creation amount",
						.actual = to_string(accountBalance.balanceGddCent),
						.expected = ">= " + to_string(creation.amountGddCent)
					};
				}
				if (accountBalance.coinCommunityIdIndex != tx.txCommunityIdIndex) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Creation account balances aren't allowed to have diff coin community id",
						.actual = to_string(accountBalance.coinCommunityIdIndex),
						.expected = to_string(tx.txCommunityIdIndex)
					};
				}
				if (accountBalance.publicKeyIndex == creation.recipientPublicKeyIndex) {
					if (creationAccountBalanceFound) {
						return { .type = ErrorType::Field_Value_Conflict, .message = "more than one account balance for creation recipient" };
					}
					creationAccountBalanceFound = true;
				}
			}
			if (!creationAccountBalanceFound) {
				return { .type = ErrorType::Field_Value_Conflict, .message = "missing account balance for creation recipient" };
			}
			return { .type = ErrorType::Success };
		}

		static Error validateSingle(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{			
			const auto& creation = tx.specific.creation;
			const auto& publicKeyIdDict = appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain()->getPublicKeyDictionary();
			if (!appContext.hasPublicKey(tx.getRecipient())) {
				return { .type = ErrorType::Invalid_Dictionary_Index, .message = "couldn't find recipient public key in Dictionary" };
			}
			if (creation.amountGddCent < 2'000) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "creation amount to low, min 0.2 GDD (2000 GDD Cent)",
					.actual = to_string(creation.amountGddCent),
					.expected = ">= 2000"
				};
			}
			if (creation.amountGddCent > 10'000'000) {
				return {
					.type = ErrorType::Invalid_Field,
					.message = "creation amount to high, max 1000 per month (10'000'000 GDD Cent)",
					.actual = to_string(creation.amountGddCent),
					.expected = "<= 10'000'000"
				};
			}
			Error result;
			if (tx.hasColdData()) {
				const auto& coldData = tx.coldData.get();
				result = validateSignatures(creation, tx.coldData.get(), publicKeyIdDict);
			}
			if (ErrorType::Success != result.type) {
				return result;
			}

			if (tx.isConfirmedTx()) {
				if (tx.txNr < 3) {
					return {
						.type = ErrorType::Invalid_Field,
						.message = "Creation must be tx nr 3 at least to have community root, recipient and someone to sign registered first",
						.actual = to_string(tx.txNr),
						.expected = "< 3"
					};
				}
				result = validateAccountBalancesSingle(tx);
			}
			return result;
		}

		Error validateGradidoCreation(const ConfirmedGradidoTx& tx, const AppContext& appContext, Options options)
		{
			if (!tx.isCreation()) {
				throw GradidoNodeInvalidDataException("called validateSingleCreation with not creation typed tx");
			}
			if (!appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain().get()) {
				return {
					.type = ErrorType::Missing_Blockchain,
					.message = "missing blockchain for community " + to_string(tx.txCommunityIdIndex)
				};
			}
			const auto& blockchain = appContext.getCommunityContext(tx.txCommunityIdIndex).getBlockchain();
			Error result;
			if ((Type::SINGLE & options.type) == Type::SINGLE) {
				result = validateSingle(tx, appContext, options);
				if (ErrorType::Success != result.type) {
					return result;
				}
			}
			if ((Type::MONTH_RANGE & options.type) == Type::MONTH_RANGE) {
				result = validateMonthRange(tx, *blockchain);
			}
			return result;
		}
	}
}
