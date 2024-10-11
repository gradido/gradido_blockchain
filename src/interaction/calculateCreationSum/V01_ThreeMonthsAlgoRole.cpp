#include "gradido_blockchain/interaction/calculateCreationSum/V01_ThreeMonthsAlgoRole.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"

#include <chrono>
#include <cassert>

using namespace std::chrono;

namespace gradido {
	namespace interaction {
		namespace calculateCreationSum {
			GradidoUnit V01_ThreeMonthsAlgoRole::run(const blockchain::Abstract& blockchain) const
			{
				GradidoUnit sum; // default initialized with zero

				// received = max
				// received - 2 month = min
				auto beforeReceived = mDate - months(2);

				blockchain.findAll(blockchain::Filter(
					// static filter
					mTransactionNrMax,
					mPublicKey,
					TimepointInterval(beforeReceived, mDate),
					// dynamic filter
					// called for each transaction which fulfills the static filters
					[&sum](const blockchain::TransactionEntry& entry) -> blockchain::FilterResult
					{
						auto body = entry.getTransactionBody();
						if (body->isCreation())
						{
							sum += body->getCreation()->getRecipient().getAmount();
						}
						// we don't need any of it in our result set
						return blockchain::FilterResult::DISMISS;
					}
				));
				return sum;
			}
		}
	}
}