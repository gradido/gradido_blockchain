#include "gradido_blockchain/interaction/calculateCreationSum/V01_ThreeMonthsAlgoRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/FilterResult.h"
#include "gradido_blockchain/blockchain/PublicKeySearchType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain_core/types/transaction.h"

#include <chrono>
#include <cassert>
#include "date/date.h"

using namespace std::chrono;

namespace gradido {
	using blockchain::CompactFilter, blockchain::FilterResult, blockchain::PublicKeySearchType;
	using blockchain::Abstract;
	using data::compact::ConfirmedGradidoTx;

	namespace interaction {
		namespace calculateCreationSum {
			GradidoUnit V01_ThreeMonthsAlgoRole::run(const Abstract& blockchain) const
			{
				auto sum(GradidoUnit::zero()); // default initialized with zero

				// received = max
				// received - 2 month = min
				auto dateYM = timepointAsYearMonth(mDate);
				auto beforeReceivedYM = dateYM - date::months(2);
				
				CompactFilter filter;
				filter.maxTransactionNr = mTransactionNrMax;
				filter.publicKeyIndex = mPublicKey;
				filter.publicKeySearchType = PublicKeySearchType::BalanceChangingPublicKey;
				filter.timepointInterval = { beforeReceivedYM, dateYM };
				filter.transactionType = GRDT_TRANSACTION_CREATION;
				
				blockchain.findAll(filter,
					[&sum, dateYM](const ConfirmedGradidoTx& tx) -> FilterResult
					{
						if (!tx.isCreation()) {
							throw GradidoNullPointerException("transaction isn't creation or invalid", "GradidoCreation", __FUNCTION__);
						}
						auto confirmedYmd = timepointAsYearMonthDay(tx.getConfirmedAt().getAsTimepoint());
						if (
							(confirmedYmd.year() == dateYM.year() && dateYM.month() - confirmedYmd.month() <= date::months(2)) ||
							(dateYM.year() - confirmedYmd.year() == date::years(1) && confirmedYmd.month() - date::months(10) == dateYM.month())
							) {
							sum += tx.getAmount();
						}
						return FilterResult::DISMISS;
					}
				);
				return sum;
			}
		}
	}
}