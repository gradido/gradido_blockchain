#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/GradidoCreationRole.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/calculateAccountBalance/GradidoTransferRole.h"

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
					std::list<std::pair<DecayDecimal, Timepoint>> receiveTransfers;
					auto lastTransactionEntryWithFinalBalance = mBlockchain->findOne(Filter(
						maxTransactionNr,
						publicKey,
						SearchDirection::DESC,
						coinCommunityId,
						[balanceDate, this, publicKey, &receiveTransfers](const TransactionEntry& entry) -> FilterFunctionResult
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
							auto balance = role->getBalance();
							if (balance) {
								receiveTransfers.push_front({ balance, confirmedTransaction->confirmedAt.getAsTimepoint() });
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
					else if (receiveTransfers.size()) {
						// if no lastGradidoBlockWithFinalBalance was found because sender is deferred transfer or not registered
						// use first received transfer as starting point
						auto firstReceived = receiveTransfers.front();
						gdd = firstReceived.first;
						lastDate = firstReceived.second;
					}

					// check for time outed deferred transfers which will be automatic booked back
					auto deferredTransfers = getTimeoutedDeferredTransferReturnedAmounts(addressIndex, lastDate, date);
					if (deferredTransfers.size()) {
						auto deferredTransfersIt = deferredTransfers.begin();
						for (auto it = receiveTransfers.begin(); it != receiveTransfers.end(); it++) {
							while (it->second > deferredTransfersIt->second && deferredTransfersIt != deferredTransfers.end()) {
								receiveTransfers.insert(it, *deferredTransfersIt);
								deferredTransfersIt++;

							}
							if (deferredTransfersIt == deferredTransfers.end()) {
								break;
							}
						}
					}

					for (auto it = receiveTransfers.begin(); it != receiveTransfers.end(); it++) 
					{
						assert(it->second >= lastDate);
						if (it->second > lastDate) {
							gdd = gdd.calculateDecay(lastDate, it->second);
							lastDate = it->second;
						}
						gdd += it->first;
					}
					// cmp return 0 if gdd == 0
					if (!gdd) {
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
						return std::make_shared<GradidoCreationRole>(body.creation); 
					}
					if (body.isDeferredTransfer()) {
						return std::make_shared<GradidoDeferredTransferRole>(body.deferredTransfer, balanceDate);
					}
					if (body.isTransfer()) {
						return std::make_shared<GradidoTransferRole>(body.transfer);
					}
					return nullptr;
				}
			}
		}
	}
}