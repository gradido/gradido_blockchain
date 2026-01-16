#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/GradidoTransfer.h"
#include "gradido_blockchain/interaction/validate/LocalGradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"

#include <cassert>
#include <memory>

#include "date/date.h"

using std::shared_ptr;

namespace gradido {
	using blockchain::Filter;
	using data::AddressType, data::ConfirmedTransaction, data::GradidoTransfer;
	namespace interaction {
		namespace validate {

			LocalGradidoTransferRole::LocalGradidoTransferRole(shared_ptr<const GradidoTransfer> gradidoTransfer)
				: mGradidoTransfer(gradidoTransfer)
			{
				assert(gradidoTransfer);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(gradidoTransfer->getSender().getPublicKey());
			}

			void LocalGradidoTransferRole::run(
				Type type,
				shared_ptr<blockchain::Abstract> blockchain,
				shared_ptr<const ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
				shared_ptr<const ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
			) {
				TransferAmountRole transferAmountRole(mGradidoTransfer->getSender());
				transferAmountRole.run(type, blockchain, ownBlockchainPreviousConfirmedTransaction, otherBlockchainPreviousConfirmedTransaction);
				auto& sender = mGradidoTransfer->getSender();

				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					validateEd25519PublicKey(mGradidoTransfer->getRecipient(), "recipient");

					if (mGradidoTransfer->getRecipient()->isTheSame(sender.getPublicKey())) {
						throw TransactionValidationException("sender and recipient are the same");
					}
				}

				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					assert(blockchain);
					std::shared_ptr<blockchain::Abstract> recipientBlockchain;
					if (!ownBlockchainPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on blockchain");
					}
					validateAccount(
						*ownBlockchainPreviousConfirmedTransaction,
						blockchain
					);
				}

				if ((type & Type::PREVIOUS) == Type::PREVIOUS)
				{
					if (!ownBlockchainPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on blockchain");
					}
				}

				if ((type & Type::PREVIOUS_BALANCE) == Type::PREVIOUS_BALANCE)
				{
					validatePrevious(*ownBlockchainPreviousConfirmedTransaction, blockchain);
				}
			}

			void LocalGradidoTransferRole::validatePrevious(
				const ConfirmedTransaction& previousConfirmedTransaction,
				shared_ptr<blockchain::Abstract> blockchain
			) {
				assert(blockchain);
				assert(mConfirmedAt.getSeconds());
				calculateAccountBalance::Context c(blockchain);
				auto& sender = mGradidoTransfer->getSender();
				auto finalBalance = c.fromEnd(
					sender.getPublicKey(),
					mConfirmedAt, // calculate decay after last transaction balance until confirmation date
					sender.getCommunityId(),
					previousConfirmedTransaction.getId() // calculate until this transaction nr
				);
					
				if (sender.getAmount() > finalBalance + GradidoUnit::fromGradidoCent(100)) {
					throw InsufficientBalanceException("not enough Gradido Balance for send coins", sender.getAmount(), finalBalance);
				}
			}

			void LocalGradidoTransferRole::validateAccount(
				const ConfirmedTransaction& ownBlockchainPreviousConfirmedTransaction,
				shared_ptr<blockchain::Abstract> blockchain
			) {
				assert(blockchain);
				Filter filter;
				filter.involvedPublicKey = mGradidoTransfer->getSender().getPublicKey();
				filter.maxTransactionNr = ownBlockchainPreviousConfirmedTransaction.getId();

				// check if sender address was registered
				auto senderAddressType = blockchain->getAddressType(filter);
				if (AddressType::NONE == senderAddressType) {
					throw WrongAddressTypeException(
						"sender address not registered", 
						senderAddressType, 
						mGradidoTransfer->getSender().getPublicKey()
					);
				}
				if (AddressType::DEFERRED_TRANSFER == senderAddressType) {
					throw WrongAddressTypeException(
						"sender address is deferred transfer, please use redeemDeferredTransferTransaction for that",
						senderAddressType,
						mGradidoTransfer->getSender().getPublicKey()
					);
				}

				// check if recipient address was registered
				filter.involvedPublicKey = mGradidoTransfer->getRecipient();
				auto recipientAddressType = blockchain->getAddressType(filter);
				if (AddressType::NONE == recipientAddressType) {
					throw WrongAddressTypeException("recipient address not registered", recipientAddressType, mGradidoTransfer->getRecipient());
				}
				if (AddressType::DEFERRED_TRANSFER == recipientAddressType) {
					throw WrongAddressTypeException("recipient cannot be a deferred transfer address", recipientAddressType, mGradidoTransfer->getRecipient());
				}
			}
		}
	}
}