#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"

#include <cassert>

#include "date/tz.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoTransferRole::GradidoTransferRole(std::shared_ptr<const data::GradidoTransfer> gradidoTransfer, std::string_view otherCommunity)
				: mGradidoTransfer(gradidoTransfer), mOtherCommunity(otherCommunity)
			{
				assert(gradidoTransfer);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(gradidoTransfer->getSender().getPublicKey());
			}

			void GradidoTransferRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				TransferAmountRole transferAmountRole(mGradidoTransfer->getSender());
				transferAmountRole.run(type, blockchain, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);
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
					if (!mOtherCommunity.empty() && mOtherCommunity != blockchain->getCommunityId()) {
						recipientBlockchain = findBlockchain(blockchain->getProvider(), mOtherCommunity, __FUNCTION__);
					}
					else {
						recipientBlockchain = blockchain;
						recipientPreviousConfirmedTransaction = senderPreviousConfirmedTransaction;
					}
					if (!senderPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on sender blockchain");
					}
					if (!recipientPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on recipient blockchain");
					}
					validateAccount(
						*senderPreviousConfirmedTransaction,
						*recipientPreviousConfirmedTransaction,
						blockchain,
						recipientBlockchain
					);
				}

				if ((type & Type::PREVIOUS) == Type::PREVIOUS)
				{
					if (!senderPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on blockchain");
					}
					validatePrevious(*senderPreviousConfirmedTransaction, blockchain);
				}
			}

			void GradidoTransferRole::validatePrevious(
				const data::ConfirmedTransaction& previousConfirmedTransaction,
				std::shared_ptr<blockchain::Abstract> blockchain
			) {
				assert(blockchain);
				assert(mConfirmedAt.getSeconds());
				calculateAccountBalance::Context c(blockchain);
				auto& sender = mGradidoTransfer->getSender();
				auto finalBalance = c.fromEnd(
					sender.getPublicKey(),
					mConfirmedAt, // calculate decay after last transaction balance until confirmation date
					previousConfirmedTransaction.getId() // calculate until this transaction nr
				);
					
				if (sender.getAmount() > finalBalance) {
					throw InsufficientBalanceException("not enough Gradido Balance for send coins", sender.getAmount(), finalBalance);
				}
			}

			void GradidoTransferRole::validateAccount(
				const data::ConfirmedTransaction& senderPreviousConfirmedTransaction,
				const data::ConfirmedTransaction& recipientPreviousConfirmedTransaction,
				std::shared_ptr<blockchain::Abstract> senderBlockchain,
				std::shared_ptr<blockchain::Abstract> recipientBlockchain
			) {
				assert(senderBlockchain);
				assert(recipientBlockchain);
				blockchain::FilterBuilder filterBuilder;

				// check if sender address was registered
				auto senderAddressType = senderBlockchain->getAddressType(
					filterBuilder
					.setInvolvedPublicKey(mGradidoTransfer->getSender().getPublicKey())
					.setMaxTransactionNr(senderPreviousConfirmedTransaction.getId())
					.build()
				);
				if (data::AddressType::NONE == senderAddressType) {
					throw WrongAddressTypeException(
						"sender address not registered", 
						senderAddressType, 
						mGradidoTransfer->getSender().getPublicKey()
					);
				}
				if (data::AddressType::DEFERRED_TRANSFER == senderAddressType) {
					throw WrongAddressTypeException(
						"sender address is deferred transfer, please use redeemDeferredTransferTransaction for that",
						senderAddressType,
						mGradidoTransfer->getSender().getPublicKey()
					);
				}

				// check if recipient address was registered
				auto recipientAddressType = recipientBlockchain->getAddressType(
					filterBuilder
					.setInvolvedPublicKey(mGradidoTransfer->getRecipient())
					.setMaxTransactionNr(recipientPreviousConfirmedTransaction.getId())
					.build()
				);
				if (data::AddressType::NONE == recipientAddressType) {
					throw WrongAddressTypeException("recipient address not registered", recipientAddressType, mGradidoTransfer->getRecipient());
				}
				if (data::AddressType::DEFERRED_TRANSFER == recipientAddressType) {
					throw WrongAddressTypeException("recipient cannot be a deferred transfer address", recipientAddressType, mGradidoTransfer->getRecipient());
				}
			}
		}
	}
}