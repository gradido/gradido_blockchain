#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/AbstractRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoRedeemDeferredTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoTimeoutDeferredTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/RegisterAddressRole.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	using namespace blockchain;
	using namespace data;

	namespace interaction {
		namespace calculateAccountBalance {

			GradidoUnit Context::run(uint64_t startTransactionNr, memory::ConstBlockPtr publicKey, Timepoint endDate) const
			{
				FilterBuilder builder;
				GradidoUnit balance(GradidoUnit::zero());
				Timepoint lastDate;
				mBlockchain->findAll(builder
					.setInvolvedPublicKey(publicKey)
					.setMinTransactionNr(startTransactionNr)
					.setSearchDirection(SearchDirection::ASC)
					.setFilterFunction([&](const TransactionEntry& entry) -> FilterResult {
						auto confirmedTransaction = entry.getConfirmedTransaction();
						if (confirmedTransaction->getConfirmedAt().getAsTimepoint() > endDate) {
							return FilterResult::STOP;
						}
						if (GradidoUnit::zero() == balance) {
							balance = confirmedTransaction->getAccountBalance(publicKey).getBalance();
							lastDate = confirmedTransaction->getConfirmedAt();
						}
						else {
							auto role = getRole(entry.getTransactionBody(), confirmedTransaction->getConfirmedAt());
							auto confirmedAt = confirmedTransaction->getConfirmedAt();
							balance = balance.calculateDecay(lastDate, confirmedAt) + role->getAmountAdded(publicKey) - role->getAmountCost(publicKey);
							lastDate = confirmedAt;
						}
						return FilterResult::DISMISS;
					})
					.build()
				);
				return balance.calculateDecay(lastDate, endDate);
			}

			

			std::shared_ptr<AbstractRole> Context::getRole(std::shared_ptr<const data::TransactionBody> body, Timepoint confirmedAt) const
			{
				// attention! work only if order in enum don't change
				static const std::array<std::function<std::shared_ptr<AbstractRole>()>, enum_integer(TransactionType::MAX_VALUE)> roleCreators = {
					[&]() { return make_shared<GradidoCreationRole>(body, mBlockchain); },
					[&]() { return make_shared<GradidoTransferRole>(body); },
					[&]() { return nullptr; },
					[&]() { return make_shared<RegisterAddressRole>(body); },
					[&]() { return make_shared<GradidoDeferredTransferRole>(body); },
					[&]() { return nullptr; },
					[&]() { return make_shared<GradidoRedeemDeferredTransferRole>(body, confirmedAt, mBlockchain); },
					[&]() { return make_shared<GradidoTimeoutDeferredTransferRole>(body, mBlockchain); }
				};
				return roleCreators[enum_integer(body->getTransactionType())]();
			}
		}
	}
}
