#include "gradido_blockchain/interaction/calculateCreationSum/V01_ThreeMonthsAlgoRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/PublicKeySearchType.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/types.h"

#include <chrono>
#include <cassert>
#include "date/date.h"

using namespace std::chrono;

namespace gradido {
	using blockchain::CompactFilter, blockchain::PublicKeySearchType;
	using blockchain::Abstract;
	using data::TransactionType;

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
				filter.transactionType = TransactionType::CREATION;

				auto txs = blockchain.findAll(filter);
				for (const auto& txRef : txs) {
					const auto& tx = txRef.get();
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
				}
				return sum;
			}
		}
	}
}