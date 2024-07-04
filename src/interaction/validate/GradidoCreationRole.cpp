#include "gradido_blockchain/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoCreationRole::GradidoCreationRole(const data::GradidoCreation& gradidoCreation)
				: mGradidoCreation(gradidoCreation)
			{
				// prepare for signature check
				mMinSignatureCount = 1;
				mForbiddenSignPublicKeys.push_back(gradidoCreation.recipient.pubkey);
			}

			void GradidoCreationRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				const auto& recipient = mGradidoCreation.recipient;
				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					validateEd25519PublicKey(recipient.pubkey, "recipient pubkey");
					auto recipientAmount = recipient.amount;
					if (recipientAmount > GradidoUnit(1000.0)) {
						throw TransactionValidationInvalidInputException("creation amount to high, max 1000 per month", "amount", "string");
					}
					if (recipientAmount < GradidoUnit(1.0)) {
						throw TransactionValidationInvalidInputException("creation amount to low, min 1 GDD", "amount", "string");
					}
				}

				if (recipient.communityId == communityId) {
					throw TransactionValidationInvalidInputException(
						"coin communityId shouldn't be set if it is the same as blockchain communityId",
						"communityId", "hex"
					);
				}

				if ((type & Type::MONTH_RANGE) == Type::MONTH_RANGE)
				{
					assert(blockchainProvider);
					auto blockchain = blockchainProvider->findBlockchain(communityId);
					assert(blockchain);
					if (!recipientPreviousConfirmedTransaction) {
						recipientPreviousConfirmedTransaction = senderPreviousConfirmedTransaction;
					}
					assert(recipientPreviousConfirmedTransaction);
					assert(mConfirmedAt.seconds);

					GradidoUnit sum;
					auto creationMaxAlgo = getCorrectCreationMaxAlgo(mConfirmedAt);
					auto ymd = date::year_month_day{ date::floor<date::days>(mGradidoCreation.targetDate.getAsTimepoint()) };
					auto targetCreationMaxAlgo = getCorrectCreationMaxAlgo(mGradidoCreation.targetDate);

					if (CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo) {
						sum = calculateCreationSumLegacy(
							mGradidoCreation.recipient.pubkey,
							mConfirmedAt,
							blockchain,
							recipientPreviousConfirmedTransaction->id
						);
					}
					else if (CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo) {
						sum = calculateCreationSum(
							mGradidoCreation.recipient.pubkey,
							ymd.month(), ymd.year(),
							mConfirmedAt, blockchain,
							recipientPreviousConfirmedTransaction->id
						);
					}
					sum += recipient.amount;
					// first max creation check algo
					if (CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == creationMaxAlgo && sum > GradidoUnit(3000.0)) {
						sum -= recipient.amount;
						throw InvalidCreationException(
							"creation more than 3.000 GDD in 3 month not allowed",
							static_cast<uint32_t>(ymd.month()), static_cast<int>(ymd.year()),
							recipient.amount, sum
						);
					}
					// second max creation check algo
					else if (CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE == creationMaxAlgo && sum > GradidoUnit(1000.0)) {
						sum -= recipient.amount;
						throw InvalidCreationException(
							"creation more than 1.000 GDD per month not allowed",
							static_cast<uint32_t>(ymd.month()), static_cast<int>(ymd.year()),
							recipient.amount, sum
						);
					}
				}
				// TODO: check if signer belongs to blockchain
				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					assert(blockchainProvider);
					auto blockchain = blockchainProvider->findBlockchain(communityId);
					assert(blockchain);

					blockchain::Filter filter;
					filter.involvedPublicKey = mGradidoCreation.recipient.pubkey;
					auto addressType = blockchain->getAddressType(filter);
					if (data::AddressType::COMMUNITY_HUMAN != addressType &&
						data::AddressType::COMMUNITY_AUF != addressType &&
						data::AddressType::COMMUNITY_GMW != addressType) {
						throw WrongAddressTypeException("wrong address type for creation", addressType, mGradidoCreation.recipient.pubkey);
					}
				}

			}

			void GradidoCreationRole::validateTargetDate(Timepoint createdAtTimePoint)
			{
				auto target_date = date::year_month_day{ date::floor<date::days>(mGradidoCreation.targetDate.getAsTimepoint()) };
				auto received = date::year_month_day{ date::floor<date::days>(createdAtTimePoint) };

				auto targetDateReceivedDistanceMonth = getTargetDateReceivedDistanceMonth(createdAtTimePoint);
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
			}

			GradidoUnit GradidoCreationRole::calculateCreationSum(
				memory::ConstBlockPtr accountPubkey,
				date::month month,
				date::year year,
				Timepoint received,
				std::shared_ptr<blockchain::Abstract> blockchain,
				uint64_t maxTransactionNr
			)
			{
				assert(blockchain);

				GradidoUnit sum; // default initialized with zero

				// received = max
				// received - 2 month = min
				auto beforeReceived = received - std::chrono::months(getTargetDateReceivedDistanceMonth(received));

				blockchain->findAll(blockchain::Filter(
					// static filter
					maxTransactionNr,
					accountPubkey,
					TimepointInterval(beforeReceived, received),
					// dynamic filter
					// called for each transaction which fulfills the static filters
					[&sum, month, year](const blockchain::TransactionEntry& entry) -> blockchain::FilterResult
					{
						auto body = entry.getTransactionBody();
						if (body->isCreation())
						{
							auto creation = body->creation;
							auto targetDate = date::year_month_day{ date::floor<date::days>(creation->targetDate.getAsTimepoint()) };
							if (targetDate.month() == month && targetDate.year() == year) {
								sum += creation->recipient.amount;
							}
						}
						// we don't need any of it in our result set
						return blockchain::FilterResult::DISMISS;
					}
				));
				return sum;
			}

			GradidoUnit GradidoCreationRole::calculateCreationSumLegacy(
				memory::ConstBlockPtr accountPubkey,
				Timepoint received,
				std::shared_ptr<blockchain::Abstract> blockchain,
				uint64_t maxTransactionNr
			)
			{
				assert(blockchain);
				// check that is is indeed an old transaction from before Sun May 03 2020 11:00:08 GMT+0000
				auto algo = getCorrectCreationMaxAlgo(received);
				assert(CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD == algo);
				auto searchDate = date::year_month_day{ date::floor<date::days>(received) };
				GradidoUnit sum; // default initialized with zero

				// received = max
				// received - 2 month = min
				auto beforeReceived = received - std::chrono::months(2);

				blockchain->findAll(blockchain::Filter(
					// static filter
					maxTransactionNr,
					accountPubkey,
					TimepointInterval(beforeReceived, received),
					// dynamic filter
					// called for each transaction which fulfills the static filters
					[&sum, &searchDate](const blockchain::TransactionEntry& entry) -> blockchain::FilterResult
					{
						auto body = entry.getTransactionBody();
						if (body->isCreation())
						{
							sum += body->creation->recipient.amount;
						}
						// we don't need any of it in our result set
						return blockchain::FilterResult::DISMISS;
					}
				));
				return sum;
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

			CreationMaxAlgoVersion GradidoCreationRole::getCorrectCreationMaxAlgo(const data::TimestampSeconds& timepoint)
			{
				if (timepoint.seconds < 1588503608) {
					return CreationMaxAlgoVersion::v01_THREE_MONTHS_3000_GDD;
				}
				return CreationMaxAlgoVersion::v02_ONE_MONTH_1000_GDD_TARGET_DATE;
			}
		}
	}
}
