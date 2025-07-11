#include "gradido_blockchain/interaction/calculateCreationSum/V02_TargetDateAlgoRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"

#include "date/tz.h"

#include <chrono>
#include <cassert>

using namespace std::chrono;

namespace gradido {
	namespace interaction {
		namespace calculateCreationSum {

			GradidoUnit V02_TargetDateAlgoRole::run(const blockchain::Abstract& blockchain) const
			{
				GradidoUnit sum; // default initialized with zero

				// received = max
				// received - 2 month = min
				auto beforeReceived = mDate - std::chrono::months(getTargetDateReceivedDistanceMonth(mDate));
				auto ymd = date::year_month_day{ date::floor<date::days>(mTargetDate) };

				blockchain.findAll(blockchain::Filter(
					// static filter
					mTransactionNrMax,
					mPublicKey,
					TimepointInterval(beforeReceived, mDate),
					// dynamic filter
					// called for each transaction which fulfills the static filters
					[&sum, ymd](const blockchain::TransactionEntry& entry) -> blockchain::FilterResult
					{
						auto body = entry.getTransactionBody();
						if (body->isCreation())
						{
							auto creation = body->getCreation();
							auto targetDate = date::year_month_day{ date::floor<date::days>(creation->getTargetDate().getAsTimepoint()) };
							if (targetDate.month() == ymd.month() && targetDate.year() == ymd.year()) {
								sum += creation->getRecipient().getAmount();
							}
						}
						// we don't need any of it in our result set
						return blockchain::FilterResult::DISMISS;
					}
				));
				return sum;
			}

			unsigned V02_TargetDateAlgoRole::getTargetDateReceivedDistanceMonth(Timepoint createdAt)
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