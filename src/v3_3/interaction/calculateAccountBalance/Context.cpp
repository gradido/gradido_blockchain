#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/GradidoCreationRole.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/GradidoTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/RegisterAddressRole.h"

namespace gradido {
	namespace v3_3 {
		using namespace blockchain;

		namespace interaction {
			namespace calculateAccountBalance {
				DecayDecimal Context::run(
					memory::ConstBlockPtr publicKey,
					Timepoint balanceDate, 
					uint64_t maxTransactionNr/* = 0 */,
					std::string_view coinCommunityId/* = nullptr*/)
				{
					// get last transaction entry with final balance 
					// collect balances with balance date from all received transactions which occurred after last transaction entry with final balance
					std::map<Timepoint, DecayDecimal> dateAmount;
					auto lastTransactionEntryWithFinalBalance = mBlockchain->findOne(Filter(
						maxTransactionNr,
						publicKey,
						SearchDirection::DESC,
						coinCommunityId,
						[balanceDate, this, publicKey, &dateAmount](const TransactionEntry& entry) -> FilterFunctionResult
						{
							auto confirmedTransaction = entry.getConfirmedTransaction();
							
							if (confirmedTransaction->confirmedAt.getAsTimepoint() > balanceDate) {
								return FilterFunctionResult::DISMISS;
							}
							auto transactionBody = confirmedTransaction->gradidoTransaction->getTransactionBody();
							auto role = getRole(*transactionBody, balanceDate);
							if (role->isFinalBalanceForAccount(publicKey)) {
								return FilterFunctionResult::USE | FilterFunctionResult::STOP;
							}
							auto amount = role->getAmount();
							if (amount > 0.0) {
								//receiveTransfers.push_front({ balance, confirmedTransaction->confirmedAt.getAsTimepoint() });
								dateAmount.insert({ confirmedTransaction->confirmedAt.getAsTimepoint() , amount });
							}
							
							// publicKey must be the receive address from deferred transfer
							// if it were sender address, isFinalBalanceForAccount should return true and we were never here
							// the receive address from deferred transfer exist only once and cannot receive more gdds
							// so we can stop here now
							if (transactionBody->isDeferredTransfer()) {
								return FilterFunctionResult::STOP;
							}
							return FilterFunctionResult::DISMISS;
						}
					));

					// define start balance and date
					Timepoint lastDate;
					DecayDecimal gdd;
					if (lastTransactionEntryWithFinalBalance) {
						auto confirmedTransaction = lastTransactionEntryWithFinalBalance->getConfirmedTransaction();
						lastDate = confirmedTransaction->confirmedAt.getAsTimepoint();
						gdd = confirmedTransaction->accountBalance;
					}
					else if (dateAmount.size()) {
						// if no lastGradidoBlockWithFinalBalance was found because sender is deferred transfer or not registered
						// use first received transfer as starting point
						auto firstReceived = dateAmount.begin();
						gdd = firstReceived->second;
						lastDate = firstReceived->first;
					}

					// check for time outed deferred transfers which will be automatic booked back
					auto timeoutedDeferredTransfers = mBlockchain->findTimeoutedDeferredTransfersInRange(
						publicKey,
						lastDate,
						balanceDate,
						maxTransactionNr
					);
					for (auto transactionEntry : timeoutedDeferredTransfers) {
						if (transactionEntry->getConfirmedTransaction()->confirmedAt.getAsTimepoint() >= lastDate) {
							continue;
						}
						dateAmount.insert(calculateBookBackTimeoutedDeferredTransfer(transactionEntry));
					}

					// check for redeemed deferred Transfer ins Range an book back the rest blocked gdd for decay
					// findRedeemedDeferredTransfersInRange
					auto deferredRedeemingTransferPairs = mBlockchain->findRedeemedDeferredTransfersInRange(
						publicKey,
						lastDate,
						balanceDate,
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
					if (gdd == Decimal(0.0)) {
						return gdd;
					}
					assert(balanceDate >= lastDate);
					auto durationUntilBalanceDate = calculateDecayDurationSeconds(lastDate, balanceDate);
					if (std::chrono::duration_cast<std::chrono::seconds>(durationUntilBalanceDate).count() >= 1) {
						gdd = gdd.calculateDecay(durationUntilBalanceDate);
					}
					return gdd;
				}

				std::shared_ptr<AbstractRole> Context::getRole(const data::TransactionBody& body, Timepoint balanceDate)
				{
					if (body.isCreation()) { 
						return std::make_shared<GradidoCreationRole>(*body.creation); 
					}
					if (body.isDeferredTransfer()) {
						return std::make_shared<GradidoDeferredTransferRole>(*body.deferredTransfer, balanceDate);
					}
					if (body.isTransfer()) {
						return std::make_shared<GradidoTransferRole>(*body.transfer);
					}
					if (body.isRegisterAddress()) {
						return std::make_shared<RegisterAddressRole>(*body.registerAddress);
					}
					return nullptr;
				}

				std::pair<Timepoint, DecayDecimal> Context::calculateBookBackTimeoutedDeferredTransfer(
					std::shared_ptr<blockchain::TransactionEntry> transactionEntry
				) {
					auto confirmedTransaction = transactionEntry->getConfirmedTransaction();
					auto body = confirmedTransaction->gradidoTransaction->getTransactionBody();
					assert(body->isDeferredTransfer());

					auto confirmedAt = confirmedTransaction->confirmedAt.getAsTimepoint();
					// use this role for calculate decayed amount at timeout 
					// confirmedAt is used as start date in GradidoDeferredTransferRole.getAmount,
					// timeout as end date
					GradidoDeferredTransferRole deferredTransferRole(*body->deferredTransfer, confirmedAt);
					auto timeout = body->deferredTransfer->timeout.getAsTimepoint();
					return { timeout, deferredTransferRole.getAmount() };
				}

				std::pair<Timepoint, DecayDecimal> Context::calculateRedeemedDeferredTransferChange(
					const blockchain::DeferredRedeemedTransferPair& deferredRedeemingTransferPair
				)
				{
					auto deferredConfirmedTransaction = deferredRedeemingTransferPair.first->getConfirmedTransaction();
					auto deferredConfirmedAt = deferredConfirmedTransaction->confirmedAt.getAsTimepoint();
					auto deferredBody = deferredConfirmedTransaction->gradidoTransaction->getTransactionBody();
					assert(deferredBody->isDeferredTransfer());
					auto deferredTransferAmount = deferredBody->deferredTransfer->transfer.sender.amount;

					auto redeemingConfirmedTransaction = deferredRedeemingTransferPair.second->getConfirmedTransaction();
					auto redeemingConfirmedAt = redeemingConfirmedTransaction->confirmedAt.getAsTimepoint();
					auto redeemingBody = redeemingConfirmedTransaction->gradidoTransaction->getTransactionBody();
					assert(redeemingBody->isTransfer());
					auto redeemingTransferAmount = redeemingBody->transfer->sender.amount;

					auto change = deferredTransferAmount.calculateDecay(deferredConfirmedAt, redeemingConfirmedAt);
					change -= redeemingTransferAmount;
					return { redeemingConfirmedAt, change };
				}
			}
		}
	}
}