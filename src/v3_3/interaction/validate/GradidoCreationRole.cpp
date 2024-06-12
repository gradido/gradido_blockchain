#include "gradido_blockchain/v3_3/interaction/validate/GradidoCreationRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"

#include "date/date.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

				void GradidoCreationRole::run(Type type = Type::SINGLE, const std::string& communityId, std::shared_ptr<AbstractBlockchainProvider> blockchainProvider)
				{

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
}