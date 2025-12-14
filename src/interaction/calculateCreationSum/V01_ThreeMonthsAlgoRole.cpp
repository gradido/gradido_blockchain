#include "gradido_blockchain/interaction/calculateCreationSum/V01_ThreeMonthsAlgoRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/serialization/toJsonString.h"

#include <chrono>
#include <cassert>
#include "date/date.h"

using namespace std::chrono;

namespace gradido {
	using blockchain::Filter;
	using blockchain::FilterBuilder;
	using blockchain::Abstract;

	namespace interaction {
		namespace calculateCreationSum {
			GradidoUnit V01_ThreeMonthsAlgoRole::run(const Abstract& blockchain) const
			{
				auto sum(GradidoUnit::zero()); // default initialized with zero

				// received = max
				// received - 2 month = min
				auto beforeReceived = mDate - months(2);
				auto ymd = date::year_month_day{ date::floor<date::days>(mDate) };

				FilterBuilder builder;
				builder
					// static filter
					.setMaxTransactionNr(mTransactionNrMax)
					.setInvolvedPublicKey(mPublicKey)
					.setTimepointInterval({ beforeReceived, mDate })
					.setTransactionType(data::TransactionType::CREATION)
					// dynamic filter
					// called for each transaction which fulfills the static filters
					.setFilterFunction(	
						[&](const blockchain::TransactionEntry& entry) -> blockchain::FilterResult
						{
							auto creation = entry.getTransactionBody()->getCreation();
							if (!creation) {
								throw GradidoNullPointerException("transaction isn't creation or invalid", "GradidoCreation", __FUNCTION__);
							}
							if (creation->getRecipient().getPublicKey()->isTheSame(mPublicKey)) {
								if (
									(entry.getYear() == ymd.year() && ymd.month() - entry.getMonth() <= date::months(2)) ||
									(ymd.year() - entry.getYear() == date::years(1) && entry.getMonth() - date::months(10) == ymd.month())
								) {
									sum += creation->getRecipient().getAmount();
								}
							}
							// we don't need any of it in our result set
							return blockchain::FilterResult::DISMISS;
						})
					;
				blockchain.findAll(builder.getFilter());
				return sum;
			}
		}
	}
}