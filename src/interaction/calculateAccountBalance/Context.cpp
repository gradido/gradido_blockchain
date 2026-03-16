#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/CompactFilter.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/blockchain/Pagination.h"
#include "gradido_blockchain/blockchain/PublicKeySearchType.h"
#include "gradido_blockchain/blockchain/TransactionRelationType.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/adapter/publicKey.h"
#include "gradido_blockchain/data/compact/ConfirmedGradidoTx.h"
#include "gradido_blockchain/data/compact/PublicKeyIndex.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TransactionType.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/AbstractRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoRedeemDeferredTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoTimeoutDeferredTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/RegisterAddressRole.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/lib/TimepointInterval.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	using data::adapter::toPublicKey;
	using data::compact::ConfirmedGradidoTx, data::compact::PublicKeyIndex;
	using data::Timestamp;
	using blockchain::CompactFilter, blockchain::Filter, blockchain::FilterBuilder, blockchain::FilterResult;
	using blockchain::Pagination, blockchain::PublicKeySearchType, blockchain::SearchDirection, blockchain::TransactionEntry;
	using namespace data;

	namespace interaction {
		namespace calculateAccountBalance {

			GradidoUnit Context::fromBegin(
				uint64_t startTransactionNr,
				memory::ConstBlockPtr publicKey,
				Timepoint endDate,
				std::optional<uint32_t> coinCommunityIdIndex
			) const 
			{
				FilterBuilder builder;
				GradidoUnit balance(GradidoUnit::zero());
				Timepoint lastDate;
				auto coinCommunityIdIndexValue = coinCommunityIdIndex.has_value()
					? coinCommunityIdIndex.value()
					: mBlockchain->getCommunityIdIndex();
				mBlockchain->findAll(builder
					.setUpdatedBalancePublicKey(publicKey)
					.setMinTransactionNr(startTransactionNr)
					.setSearchDirection(SearchDirection::ASC)
					.setCoinCommunityIdIndex(coinCommunityIdIndex)
					.setFilterFunction([&](const TransactionEntry& entry) -> FilterResult {
						auto confirmedTransaction = entry.getConfirmedTransaction();
						if (confirmedTransaction->getConfirmedAt().getAsTimepoint() > endDate) {
							return FilterResult::STOP;
						}
						if (GradidoUnit::zero() == balance) {
							balance = confirmedTransaction->getAccountBalance(publicKey, coinCommunityIdIndexValue).getBalance();
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

			// calculate balance address from last transaction found for the pubkey with transaction <= maxTransactionNr
			GradidoUnit Context::fromEnd(
				memory::ConstBlockPtr publicKey,
				Timestamp endDate,
				std::optional<uint32_t> coinCommunityIdIndex/* = nullopt*/,
				uint64_t maxTransactionNr/* = 0 */
			) const 
			{
				if (!publicKey || publicKey->isEmpty()) {
					throw GradidoNullPointerException("empty publicKey", "ConstBlockPtr", __FUNCTION__);
				}

				auto publicKeyIndex = mBlockchain->getPublicKeyDictionary().getIndexForData(toPublicKey(*publicKey));
				if (!publicKeyIndex || publicKeyIndex != (uint32_t)publicKeyIndex) {
					throw DictionaryMissingEntryException("missing public key index", publicKey->convertToHex());
				}
				PublicKeyIndex fullPublicKeyIndex = { .communityIdIndex = mBlockchain->getCommunityIdIndex(), .publicKeyIndex = (uint32_t)publicKeyIndex };
				return fromEnd(fullPublicKeyIndex, endDate, coinCommunityIdIndex, maxTransactionNr);
			}

			GradidoUnit Context::fromEnd(
				data::compact::PublicKeyIndex balanceChangingPublicKey,
				data::Timestamp endDate,
				std::optional<uint32_t> coinCommunityIdIndex /*= std::nullopt*/,
				uint64_t maxTransactionNr /*= 0*/
			) const
			{
				auto filter = CompactFilter::lastBalanceFor(balanceChangingPublicKey);
				filter.maxTransactionNr = maxTransactionNr;
				if (coinCommunityIdIndex) {
					filter.coinCommunityIdIndex = *coinCommunityIdIndex;
				}
				filter.timepointInterval = TimepointInterval(Timepoint(), endDate);
				GradidoUnit resultGdds(GradidoUnit::zero());
				mBlockchain->findAll(filter,
					[endDate, filter, &resultGdds](const ConfirmedGradidoTx& tx) -> FilterResult
					{
						if (tx.getConfirmedAt() > endDate) {
							return FilterResult::DISMISS;
						}
						resultGdds = tx.getAccountBalance(filter.publicKeyIndex, filter.coinCommunityIdIndex).getDecayedAmount(endDate);
						return FilterResult::STOP;
					}
				);
				
				return resultGdds;
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
