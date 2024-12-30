#include "gradido_blockchain/blockchain/Abstract.h"
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
			std::vector<data::AccountBalance> Context::run(
				data::ConstGradidoTransactionPtr gradidoTransaction,
				const std::map<blockchain::TransactionRelationType, std::shared_ptr<const blockchain::TransactionEntry>>& relatedTransactions,
				Timepoint confirmedAt,
				uint64_t id
			) {
				auto transactionBody = gradidoTransaction->getTransactionBody();
				
				if (transactionBody->isRegisterAddress() && transactionBody->getType() != data::CrossGroupType::LOCAL) {
					throw std::runtime_error("not implemented yet");
				}
				
				auto role = getRole(transactionBody, confirmedAt);
				if (!role) {
					throw GradidoNodeInvalidDataException("cannot find role for this transaction type");
				}
				std::vector<data::AccountBalance> resultAccountBalances;
				for (auto& it : relatedTransactions) {

				}
				if (role->getSender()) {
					resultAccountBalances.push_back(calculateAccountBalance(
						relatedTransactions[enum_integer(TransactionRelationType::SenderPrevious)],
						role->getSender(),
						confirmedAt
					));
				}
				if (role->getRecipient()) {
					resultAccountBalances.push_back(calculateAccountBalance(
						relatedTransactions[enum_integer(TransactionRelationType::RecipientPrevious)],
						role->getRecipient(),
						confirmedAt
					));
				}

				auto balance = run(role->getFinalBalanceAddress(), confirmedAt, id - 1);
				balance += role->getAmountAdded(role->getFinalBalanceAddress());
				GradidoUnit subtractAmount = role->getAmountCost(role->getFinalBalanceAddress());
				if (balance < subtractAmount) {
					throw InsufficientBalanceException(
						"not enough gdd",
						subtractAmount,
						balance
					);
				}
				balance -= subtractAmount;
				return balance;
			}

			GradidoUnit Context::run(
				memory::ConstBlockPtr publicKey,
				Timepoint balanceDate,
				uint64_t maxTransactionNr/* = 0 */,
				std::string_view coinCommunityId /* = std::string_view() */)
			{
				// get last transaction entry with final balance
				// collect balances with balance date from all received transactions which occurred after last transaction entry with final balance
				std::multimap<Timepoint, GradidoUnit> dateAmount;
				auto lastTransactionEntryWithFinalBalance = mBlockchain.findOne(Filter(
					maxTransactionNr,
					publicKey,
					SearchDirection::DESC,
					coinCommunityId,
					[balanceDate, this, publicKey, &dateAmount](const TransactionEntry& entry) -> FilterResult
					{
						auto confirmedTransaction = entry.getConfirmedTransaction();

						if (confirmedTransaction->getConfirmedAt().getAsTimepoint() > balanceDate) {
							return FilterResult::DISMISS;
						}
						auto transactionBody = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
						auto role = getRole(*transactionBody);
						if (!role) {
							return FilterResult::DISMISS;
						}
						if (role->isFinalBalanceForAccount(publicKey)) {
							return FilterResult::USE | FilterResult::STOP;
						}
						auto amount = role->getAmountAdded(publicKey);
						if (amount > GradidoUnit::zero()) {
							auto confirmedDate = confirmedTransaction->getConfirmedAt().getAsTimepoint();
							if (transactionBody->isDeferredTransfer()) {
								// if timeout is reached, balance is zero
								// if deferred transfer wasn't redeemed, it was booked back to sender,
								// if it was redeemed, it is booked away to redeem account
								auto timeout = transactionBody->getDeferredTransfer()->getTimeout().getAsTimepoint();
								if (timeout <= balanceDate) {
									return FilterResult::DISMISS;
								}
							}
							dateAmount.insert({ balanceDate, amount });
						}

						// publicKey must be the receive address from deferred transfer
						// if it were sender address, isFinalBalanceForAccount should return true and we were never here
						// the receive address from deferred transfer exist only once and cannot receive more gdds
						// so we can stop here now
						if (transactionBody->isDeferredTransfer()) {
							return FilterResult::STOP;
						}
						return FilterResult::DISMISS;
					}
				));

				// define start balance and date
				Timepoint lastDate;
				GradidoUnit gdd;
				if (lastTransactionEntryWithFinalBalance) {
					auto confirmedTransaction = lastTransactionEntryWithFinalBalance->getConfirmedTransaction();
					lastDate = confirmedTransaction->getConfirmedAt().getAsTimepoint();
					gdd = confirmedTransaction->getAccountBalance();
				}
				else if (dateAmount.size()) {
					// if no lastGradidoBlockWithFinalBalance was found because sender is deferred transfer or not registered
					// use first received transfer as starting point
					auto firstReceived = dateAmount.begin();
					gdd = firstReceived->second;
					lastDate = firstReceived->first;
					dateAmount.erase(firstReceived);
				}

				// check for time outed deferred transfers which will be automatic booked back
				auto timeoutedDeferredTransfers = mBlockchain.findTimeoutedDeferredTransfersInRange(
					publicKey,
					TimepointInterval(lastDate, balanceDate),
					maxTransactionNr
				);
				for (auto transactionEntry : timeoutedDeferredTransfers) {
					if (transactionEntry->getConfirmedTransaction()->getConfirmedAt().getAsTimepoint() >= lastDate) {
						continue;
					}
					dateAmount.insert(calculateBookBackTimeoutedDeferredTransfer(transactionEntry));
				}

				// check for redeemed deferred Transfer in Range and book back the rest blocked gdd for decay
				// findRedeemedDeferredTransfersInRange
				auto deferredRedeemingTransferPairs = mBlockchain.findRedeemedDeferredTransfersInRange(
					publicKey,
					TimepointInterval(lastDate, balanceDate),
					maxTransactionNr
				);
				for (const auto& deferredRedeemingTransferPair : deferredRedeemingTransferPairs) {
					dateAmount.insert(calculateRedeemedDeferredTransferChange(deferredRedeemingTransferPair));
				}

				// sum up received transfers and calculate decay between
				for (auto& receiveTransfer: dateAmount)
				{
					assert(receiveTransfer.first >= lastDate);
					if (receiveTransfer.first > lastDate) {
						gdd = gdd.calculateDecay(lastDate, receiveTransfer.first);
						lastDate = receiveTransfer.first;
					}
					gdd += receiveTransfer.second;
				}
				// cmp return 0 if gdd == 0
				if (gdd == GradidoUnit(0.0) || balanceDate == lastDate) {
					return gdd;
				}
				assert(balanceDate > lastDate);
				gdd = gdd.calculateDecay(lastDate, balanceDate);

				return gdd;
			}

			std::shared_ptr<AbstractRole> Context::getRole(std::shared_ptr<const data::TransactionBody> body, Timepoint confirmedAt)
			{
				// attention! work only if order in enum don't change
				static const std::array<std::function<std::shared_ptr<AbstractRole>()>, enum_integer(TransactionType::MAX_VALUE)> roleCreators = {
					[&]() { return make_shared<GradidoCreationRole>(body); },
					[&]() { return make_shared<GradidoTransferRole>(body); },
					[&]() { return nullptr; },
					[&]() { return make_shared<RegisterAddressRole>(body); },
					[&]() { return make_shared<GradidoDeferredTransferRole>(body); },
					[&]() { return nullptr; },
					[&]() { return make_shared<GradidoRedeemDeferredTransferRole>(body); },
					[&]() { return make_shared<GradidoTimeoutDeferredTransferRole>(body, confirmedAt, mBlockchain); }
				};
				return roleCreators[enum_integer(body->getTransactionType())]();
			}

			AccountBalance Context::calculateAccountBalance(
				std::shared_ptr<const blockchain::TransactionEntry> lastOwnerTransaction,
				memory::ConstBlockPtr ownerPublicKey,
				Timepoint confirmedAt
			)
			{	
				auto ownerConfirmedTransaction = lastOwnerTransaction->getConfirmedTransaction();
				auto lastOwnerTransactionConfirmedAt = ownerConfirmedTransaction->getConfirmedAt();
				auto role = getRole(lastOwnerTransaction->getTransactionBody(), lastOwnerTransactionConfirmedAt);
				if (!role) return { ownerPublicKey, 0ll };
				auto decayedBalance = ownerConfirmedTransaction->getAccountBalance(ownerPublicKey)
					.getBalance()
					.calculateDecay(lastOwnerTransactionConfirmedAt, confirmedAt)
				;
				return { ownerPublicKey, decayedBalance + role->getAmountAdded(ownerPublicKey) - role->getAmountCost(ownerPublicKey) };
			}
		}
	}
}
