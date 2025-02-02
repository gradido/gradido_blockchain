#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/interaction/calculateCreationSum/Context.h"
#include "gradido_blockchain/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoCreationRole::GradidoCreationRole(std::shared_ptr<const data::GradidoCreation> gradidoCreation)
				: mGradidoCreation(gradidoCreation)
			{
				assert(gradidoCreation);
				// prepare for signature check
				mMinSignatureCount = 1;
				mForbiddenSignPublicKeys.push_back(mGradidoCreation->getRecipient().getPublicKey());
			}

			void GradidoCreationRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				const auto& recipient = mGradidoCreation->getRecipient();
				TransferAmountRole transferAmountRole(mGradidoCreation->getRecipient());
				transferAmountRole.run(type, blockchain, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);

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
					if (recipientAmount < GradidoUnit(1.0)) {
						throw TransactionValidationInvalidInputException(
							"creation amount to low, min 1 GDD",
							"amount",
							"string",
							">= 1",
							recipientAmount.toString().data()
						);
					}
				}

				if ((type & Type::MONTH_RANGE) == Type::MONTH_RANGE)
				{
					if (!recipientPreviousConfirmedTransaction && senderPreviousConfirmedTransaction) {
						recipientPreviousConfirmedTransaction = senderPreviousConfirmedTransaction;
					}
					if (!recipientPreviousConfirmedTransaction) {
						throw GradidoNullPointerException(
							"missing previous confirmed transaction for interaction::validate Creation",
							"data::ConstConfirmedTransactionPtr",
							__FUNCTION__
						);
					}
					assert(mConfirmedAt.getSeconds());

					calculateCreationSum::Context calculateCreationSum(
						mConfirmedAt,
						mGradidoCreation->getTargetDate(),
						mGradidoCreation->getRecipient().getPublicKey(),
						recipientPreviousConfirmedTransaction->getId()
					);

					GradidoUnit sum = calculateCreationSum.run(*blockchain);
					sum += recipient.getAmount();
					if (sum > calculateCreationSum.getLimit()) {
						auto targetDate = mGradidoCreation->getTargetDate();
						auto ymd = date::year_month_day{ date::floor<date::days>(targetDate.getAsTimepoint()) };
						sum -= recipient.getAmount();
						std::string message = "creation more than ";
						message += calculateCreationSum.getLimit().toString() + " not allowed";

						throw InvalidCreationException(
							message.data(),
							static_cast<uint32_t>(ymd.month()), static_cast<int>(ymd.year()),
							recipient.getAmount(), sum
						);
					}
				}
				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					blockchain::Filter filter;
					filter.involvedPublicKey = mGradidoCreation->getRecipient().getPublicKey();
					auto addressType = blockchain->getAddressType(filter);
					if (data::AddressType::COMMUNITY_HUMAN != addressType &&
						data::AddressType::COMMUNITY_AUF != addressType &&
						data::AddressType::COMMUNITY_GMW != addressType) {
						throw WrongAddressTypeException("wrong address type for creation", addressType, mGradidoCreation->getRecipient().getPublicKey());
					}
				}
			}

			void GradidoCreationRole::checkRequiredSignatures(
				const data::SignatureMap& signatureMap,
				std::shared_ptr<blockchain::Abstract> blockchain /*  = nullptr*/
			) const
			{
				AbstractRole::checkRequiredSignatures(signatureMap, blockchain);
				if (!blockchain) return;
				auto& signPairs = signatureMap.getSignaturePairs();
				// check for account type
				for (auto& signPair : signPairs) {
					blockchain::Filter filter;
					filter.involvedPublicKey = signPair.getPublicKey();
					filter.searchDirection = blockchain::SearchDirection::DESC;
					filter.timepointInterval = TimepointInterval(blockchain->getStartDate(), mCreatedAt);
					auto signerAccountType = blockchain->getAddressType(filter);
					if (data::AddressType::COMMUNITY_HUMAN != signerAccountType) {
						throw WrongAddressTypeException(
							"signer for creation doesn't have a community human account",
							signerAccountType,
							signPair.getPublicKey()
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
							+ DataTypeConverter::timePointToString(createdAtTimePoint)
							+ " - "
							+ std::to_string(static_cast<unsigned>(targetDateReceivedDistanceMonth))
							+ " months"
						;
						throw TransactionValidationInvalidInputException(
							"year is the same, target date month is invalid",
							"target_date",
							"TimestampSeconds",
							expected.data(),
							DataTypeConverter::timePointToString(mGradidoCreation->getTargetDate().getAsTimepoint()).data()
						);
					}
					if (target_date.month() > received.month()) {
						std::string expected = "<= " + std::to_string(static_cast<unsigned>(received.month()));
						throw TransactionValidationInvalidInputException(
							"year is the same, target date month is invalid",
						 	"target_date",
							"TimestampSeconds",
							expected.data(),
							std::to_string(static_cast<unsigned>(target_date.month())).data()
						);
					}
				}
				else if (target_date.year() > received.year())
				{
					std::string expected = "<= " + std::to_string(static_cast<int>(received.year()));
					throw TransactionValidationInvalidInputException(
						"target date year is in future",
						"target_date",
						"TimestampSeconds",
						expected.data(),
						std::to_string(static_cast<int>(target_date.year())).data()
					);
				}
				else if (static_cast<int>(target_date.year()) + 1 < static_cast<int>(received.year()))
				{
					std::string expected = " >= " + std::to_string(static_cast<int>(received.year())) + " - 1 year";
					throw TransactionValidationInvalidInputException(
						"target date year is in past",
						"target_date",
						"TimestampSeconds",
						expected.data(),
						std::to_string(static_cast<int>(target_date.year())).data()
					);
				}
				else
				{
					// target_date.year +1 == now.year
					if (static_cast<unsigned>(target_date.month()) + targetDateReceivedDistanceMonth < static_cast<unsigned>(received.month()) + 12) {
						std::string expected = ">= "
							+ DataTypeConverter::timePointToString(createdAtTimePoint)
							+ " - "
							+ std::to_string(static_cast<unsigned>(targetDateReceivedDistanceMonth))
							+ " months"
						;
						throw TransactionValidationInvalidInputException(
							"target date month is invalid",
							"target_date",
							"TimestampSeconds",
							expected.data(),
							DataTypeConverter::timePointToString(mGradidoCreation->getTargetDate().getAsTimepoint()).data()
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
		}
	}
}
