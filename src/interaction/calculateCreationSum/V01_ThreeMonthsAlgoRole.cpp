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

	namespace interaction::calculateCreationSum {
		GradidoUnit v01_ThreeMonthsAlgo(
			date::year_month dateYM,
			data::compact::PublicKeyIndex publicKey,
			const blockchain::Abstract& blockchain,
			uint64_t transactionNrMax/* = 0 */
		) {
			auto sum(GradidoUnit::zero()); // default initialized with zero

			// received = max
			// received - 2 month = min
			auto beforeReceivedYM = dateYM - date::months(2);

			CompactFilter filter;
			filter.maxTransactionNr = transactionNrMax;
			filter.publicKeyIndex = publicKey;
			filter.publicKeySearchType = PublicKeySearchType::BalanceChangingPublicKey;
			filter.timepointInterval = { beforeReceivedYM, dateYM };
			filter.transactionType = TransactionType::CREATION;

			auto txs = blockchain.findAll(filter);
			for (const auto& txRef : txs) {
				const auto& tx = txRef.get();
				if (!tx->isCreation()) {
					throw GradidoNullPointerException("transaction isn't creation or invalid", "GradidoCreation", __FUNCTION__);
				}
				auto confirmedYmd = timepointAsYearMonthDay(tx->getConfirmedAt().getAsTimepoint());
				if (
					(confirmedYmd.year() == dateYM.year() && dateYM.month() - confirmedYmd.month() <= date::months(2)) ||
					(dateYM.year() - confirmedYmd.year() == date::years(1) && confirmedYmd.month() - date::months(10) == dateYM.month())
					) {
					sum += tx->getAmount();
				}
			}
			return sum;
		}
		GradidoUnit V01_ThreeMonthsAlgoRole::run(const blockchain::Abstract& blockchain) const {
			return v01_ThreeMonthsAlgo(timepointAsYearMonth(mDate), mPublicKey, blockchain, mTransactionNrMax);
		}
	}
}