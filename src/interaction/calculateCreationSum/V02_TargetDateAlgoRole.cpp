#include "gradido_blockchain/interaction/calculateCreationSum/V02_TargetDateAlgoRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/FilterResult.h"
#include "gradido_blockchain/blockchain/PublicKeySearchType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/lib/TimepointInterval.h"
#include "gradido_blockchain_core/types/transaction.h"

#include "date/date.h"

#include <chrono>
#include <cassert>

using namespace std::chrono;

namespace gradido {
	using blockchain::CompactFilter, blockchain::FilterResult, blockchain::PublicKeySearchType;
	using blockchain::Abstract;
	using data::compact::ConfirmedGradidoTx;

	namespace interaction {
		namespace calculateCreationSum {

			GradidoUnit V02_TargetDateAlgoRole::run(const blockchain::Abstract& blockchain) const
			{
				auto sum(GradidoUnit::zero());

				// received = max
				// received - 2 month = min
//				auto dateYM = timepointAsYearMonth(mDate);
//				auto beforeReceivedYM = dateYM - getTargetDateReceivedDistanceMonth(mDate);
				auto beforeReveived = mDate - getTargetDateReceivedDistanceMonth(mDate);
				auto ym = timepointAsYearMonth(mTargetDate);
				
				CompactFilter filter;
				filter.maxTransactionNr = mTransactionNrMax;
				filter.publicKeyIndex = mPublicKey;
				filter.publicKeySearchType = PublicKeySearchType::BalanceChangingPublicKey;
				filter.timepointInterval = { beforeReveived, mDate };
				filter.transactionType = GRDT_TRANSACTION_CREATION;

				blockchain.findAll(filter, 
					[&sum, ym](const ConfirmedGradidoTx& tx) -> FilterResult
					{
						if (!tx.isCreation()) {
							throw GradidoNullPointerException("transaction isn't creation or invalid", "GradidoCreation", __FUNCTION__);
						}
						auto targetDate = tx.specific.creation.targetMonthYear;
						if (targetDate.month() == ym.month() && targetDate.year() == ym.year()) {
							sum += tx.getAmount();
						}
						return FilterResult::DISMISS;
					}
				);
				return sum;
			}

			date::months V02_TargetDateAlgoRole::getTargetDateReceivedDistanceMonth(Timepoint createdAt)
			{
				date::months targetDateReceivedDistanceMonth(2);
				// extra rule from the beginning and testing phase to keep transactions from beginning valid
				// allow 3 month distance between created and target date between this dates
				// 1585544394 = Mon Mar 30 2020 04:59:54 GMT+0000
				// 1641681224 = Sat Jan 08 2022 22:33:44 GMT+0000
				auto secondsSinceEpoch = time_point_cast<std::chrono::seconds>(createdAt).time_since_epoch().count();
				if (secondsSinceEpoch > 1585544394 && secondsSinceEpoch < 1641681224) {
					targetDateReceivedDistanceMonth = date::months(3);
				}
				return targetDateReceivedDistanceMonth;
			}
		}
	}
}