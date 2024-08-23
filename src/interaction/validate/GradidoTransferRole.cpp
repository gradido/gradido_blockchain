#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"

#include "gradido_blockchain/blockchain/FilterBuilder.h"

#include <cassert>

#include "date/date.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoTransferRole::GradidoTransferRole(std::shared_ptr<const data::GradidoTransfer> gradidoTransfer, std::string_view otherCommunity)
				: mGradidoTransfer(gradidoTransfer), mOtherCommunity(otherCommunity), mDeferredTransfer(false)
			{
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(gradidoTransfer->getSender().getPubkey());
			}

			void GradidoTransferRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				TransferAmountRole transferAmountRole(mGradidoTransfer->getSender());
				transferAmountRole.run(type, communityId, blockchainProvider, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);
				auto& sender = mGradidoTransfer->getSender();

				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					validateEd25519PublicKey(mGradidoTransfer->getRecipient(), "recipient");

					if (mGradidoTransfer->getRecipient()->isTheSame(sender.getPubkey())) {
						throw TransactionValidationException("sender and recipient are the same");
					}
				}

				if ((type & Type::PREVIOUS) == Type::PREVIOUS)
				{
					if (!senderPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on blockchain");
					}
					validatePrevious(*senderPreviousConfirmedTransaction, blockchainProvider->findBlockchain(communityId));
				}
				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					auto senderBlockchain = blockchainProvider->findBlockchain(communityId);
					std::shared_ptr<blockchain::Abstract> recipientBlockchain;
					if (!mOtherCommunity.empty() && mOtherCommunity != communityId) {
						recipientBlockchain = blockchainProvider->findBlockchain(mOtherCommunity);
					}
					else {
						recipientBlockchain = senderBlockchain;
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
						senderBlockchain,
						recipientBlockchain
					);
				}
			}

			void GradidoTransferRole::validatePrevious(
				const data::ConfirmedTransaction& previousConfirmedTransaction,
				std::shared_ptr<blockchain::Abstract> blockchain
			) {
				assert(blockchain);
				assert(mConfirmedAt.getSeconds());
				calculateAccountBalance::Context c(*blockchain);
				auto& sender = mGradidoTransfer->getSender();
				auto finalBalance = c.run(
					sender.getPubkey(),
					mConfirmedAt, // calculate decay after last transaction balance until confirmation date
					previousConfirmedTransaction.getId(), // calculate until this transaction nr
					sender.getCommunityId()
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
					.setInvolvedPublicKey(mGradidoTransfer->getSender().getPubkey())
					.setMaxTransactionNr(senderPreviousConfirmedTransaction.getId())
					.build()
				);
				if (data::AddressType::NONE == senderAddressType) {
					throw WrongAddressTypeException(
						"sender address not registered", 
						senderAddressType, 
						mGradidoTransfer->getSender().getPubkey()
					);
				}

				// check if recipient address was registered
				auto recipientAddressType = recipientBlockchain->getAddressType(
					filterBuilder
					.setInvolvedPublicKey(mGradidoTransfer->getRecipient())
					.setMaxTransactionNr(recipientPreviousConfirmedTransaction.getId())
					.build()
				);
				if (data::AddressType::NONE == recipientAddressType && !mDeferredTransfer) {
					throw WrongAddressTypeException("recipient address not registered", recipientAddressType, mGradidoTransfer->getRecipient());
				}
				// with deferred transfer recipient address is completely new 
				else if(data::AddressType::NONE != recipientAddressType && mDeferredTransfer) {
					throw WrongAddressTypeException("deferred transfer address already exist", recipientAddressType, mGradidoTransfer->getRecipient());
				}
			}
		}
	}
}