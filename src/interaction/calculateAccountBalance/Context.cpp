#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/AbstractRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoCreationRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/RegisterAddressRole.h"

namespace gradido {
	using namespace blockchain;

	namespace interaction {
		namespace calculateAccountBalance {
			GradidoUnit Context::run(
				data::ConstGradidoTransactionPtr gradidoTransaction,
				Timepoint confirmedAt,
				uint64_t id
			) {
				auto transactionBody = gradidoTransaction->getTransactionBody();

				if (transactionBody->isRegisterAddress() && transactionBody->getType() != data::CrossGroupType::LOCAL) {
					throw std::runtime_error("not implemented yet");
				}
				if (transactionBody->isTransfer() || transactionBody->isCreation() || transactionBody->isDeferredTransfer()) {
					auto role = getRole(*transactionBody);
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
				return 0.0;
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

			std::shared_ptr<AbstractRole> Context::getRole(const data::TransactionBody& body)
			{
				if (body.isCreation()) {
					return std::make_shared<GradidoCreationRole>(*body.getCreation());
				}
				if (body.isDeferredTransfer()) {
					return std::make_shared<GradidoDeferredTransferRole>(body);
				}
				if (body.isTransfer()) {
					return std::make_shared<GradidoTransferRole>(*body.getTransfer());
				}
				if (body.isRegisterAddress()) {
					return std::make_shared<RegisterAddressRole>(*body.getRegisterAddress());
				}
				return nullptr;
			}

			std::pair<Timepoint, GradidoUnit> Context::calculateBookBackTimeoutedDeferredTransfer(
				std::shared_ptr<const blockchain::TransactionEntry> transactionEntry
			) {
				auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
				auto body = confirmedTransaction->getGradidoTransaction()->getTransactionBody();
				assert(body->isDeferredTransfer());

				auto confirmedAt = confirmedTransaction->getConfirmedAt().getAsTimepoint();
				// get the amount put into deferredTransfer which we can get back at timeout when not redeemd
				GradidoDeferredTransferRole deferredTransferRole(*body);
				auto timeout = body->getDeferredTransfer()->getTimeout().getAsTimepoint();
				return { timeout, deferredTransferRole.getAmountAdded(deferredTransferRole.getSender().getPubkey()) };
			}

			std::pair<Timepoint, GradidoUnit> Context::calculateRedeemedDeferredTransferChange(
				const std::pair<std::shared_ptr<const blockchain::TransactionEntry>, std::shared_ptr<const blockchain::TransactionEntry>>& deferredRedeemingTransferPair
			)
			{
				auto deferredConfirmedTransaction = deferredRedeemingTransferPair.first->getConfirmedTransaction();
				auto deferredConfirmedAt = deferredConfirmedTransaction->getConfirmedAt().getAsTimepoint();
				auto deferredBody = deferredConfirmedTransaction->getGradidoTransaction()->getTransactionBody();
				assert(deferredBody->isDeferredTransfer());
				GradidoDeferredTransferRole deferredTransferRole(*deferredBody);
				auto deferredTransferAmount = deferredTransferRole.getAmountCost(deferredBody->getDeferredTransfer()->getSenderPublicKey());

				auto redeemingConfirmedTransaction = deferredRedeemingTransferPair.second->getConfirmedTransaction();
				auto redeemingConfirmedAt = redeemingConfirmedTransaction->getConfirmedAt().getAsTimepoint();
				auto redeemingBody = redeemingConfirmedTransaction->getGradidoTransaction()->getTransactionBody();
				auto redeemingRole = getRole(*redeemingBody);
				auto redeemingTransferAmount = redeemingRole->getAmountCost(deferredBody->getDeferredTransfer()->getRecipientPublicKey());

				auto change = deferredTransferAmount.calculateDecay(deferredConfirmedAt, redeemingConfirmedAt);
				change -= redeemingTransferAmount;
				return { redeemingConfirmedAt, change };
			}
		}
	}
}
