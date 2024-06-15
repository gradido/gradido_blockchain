#include "gradido_blockchain/v3_3/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/v3_3/interaction/validate/Exceptions.h"

#include <cassert>

#include "date/date.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

				void GradidoTransferRole::run(
					Type type,
					const std::string& communityId,
					std::shared_ptr<AbstractBlockchainProvider> blockchainProvider,
					data::ConfirmedTransactionPtr senderPreviousConfirmedTransaction,
					data::ConfirmedTransactionPtr recipientPreviousConfirmedTransaction
				) {
					auto sender = mGradidoTransfer.sender;
					if ((type & Type::SINGLE) == Type::SINGLE)
					{
						validateSingle(communityId);
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
						std::shared_ptr<AbstractBlockchain> recipientBlockchain;
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

				void GradidoTransferRole::validateSingle(const std::string& communityId)
				{
					auto sender = mGradidoTransfer.sender;
					if (sender.amount <= 0) {
						throw TransactionValidationInvalidInputException("zero or negative amount", "amount", "Decimal");
					}
					validateEd25519PublicKey(mGradidoTransfer.recipient, "recipient");
					validateEd25519PublicKey(sender.pubkey, "sender");

					if (!mGradidoTransfer.recipient->isTheSame(sender.pubkey)) {
						throw TransactionValidationException("sender and recipient are the same");
					}

					if (sender.communityId == communityId) {
						throw TransactionValidationInvalidInputException(
							"coin communityId shouldn't be set if it is the same as blockchain communityId",
							"communityId", "hex"
						);
					}
				}

				void GradidoTransferRole::validatePrevious(
					const data::ConfirmedTransaction& previousConfirmedTransaction,
					std::shared_ptr<AbstractBlockchain> blockchain
				) {
					assert(blockchain);
					assert(mConfirmedAt.seconds);
					auto finalBalance = blockchain->calculateAddressBalance(
						mGradidoTransfer.sender.pubkey,
						mGradidoTransfer.sender.communityId,
						mConfirmedAt, // calculate decay after last transaction balance until confirmation date
						previousConfirmedTransaction.id // calculate until this transaction nr
					);
					
					if (mGradidoTransfer.sender.amount > finalBalance) {
						throw InsufficientBalanceException("not enough Gradido Balance for send coins", mGradidoTransfer.sender.amount, finalBalance);
					}
				}

				void GradidoTransferRole::validateAccount(
					const data::ConfirmedTransaction& senderPreviousConfirmedTransaction,
					const data::ConfirmedTransaction& recipientPreviousConfirmedTransaction,
					std::shared_ptr<AbstractBlockchain> senderBlockchain,
					std::shared_ptr<AbstractBlockchain> recipientBlockchain
				) {
					assert(senderBlockchain);
					assert(recipientBlockchain);
					// check if sender address was registered
					auto senderAddressType = senderBlockchain->getAddressType(mGradidoTransfer.sender.pubkey, senderPreviousConfirmedTransaction.id);
					if (data::AddressType::NONE == senderAddressType) {
						throw WrongAddressTypeException("sender address not registered", senderAddressType, mGradidoTransfer.sender.pubkey);
					}

					// check if recipient address was registered
					auto recipientAddressType = recipientBlockchain->getAddressType(mGradidoTransfer.recipient, recipientPreviousConfirmedTransaction.id);
					if (data::AddressType::NONE == recipientAddressType && !mDeferredTransfer) {
						throw WrongAddressTypeException("recipient address not registered", recipientAddressType, mGradidoTransfer.recipient);
					}
					// with deferred transfer recipient address is completely new 
					else if(data::AddressType::NONE != recipientAddressType && mDeferredTransfer) {
						throw WrongAddressTypeException("deferred transfer address already exist", recipientAddressType, mGradidoTransfer.recipient);
					}
				}

			}
		}
	}
}