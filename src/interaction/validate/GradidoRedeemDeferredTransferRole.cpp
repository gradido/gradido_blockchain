#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/interaction/validate/GradidoRedeemDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoRedeemDeferredTransferRole::GradidoRedeemDeferredTransferRole(
				std::shared_ptr<const data::GradidoRedeemDeferredTransfer> redeemDeferredTransfer
			) : mRedeemDeferredTransfer(redeemDeferredTransfer)
			{
				assert(redeemDeferredTransfer);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(redeemDeferredTransfer->getTransfer().getSender().getPublicKey());
			}

			void GradidoRedeemDeferredTransferRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mRedeemDeferredTransfer->getDeferredTransferTransactionNr() <= 3) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr is to short",
							"deferred_transfer_transaction_nr",
							"uint64",
							"> 3",
							std::to_string(mRedeemDeferredTransfer->getDeferredTransferTransactionNr()).data()
						);
					}
				}
				if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
					assert(blockchain);
					auto deferredTransferEntry = blockchain->getTransactionForId(mRedeemDeferredTransfer->getDeferredTransferTransactionNr());
					if (!deferredTransferEntry) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr is invalid, couldn't find transaction",
							"deferred_transfer_transaction_nr",
							"uint64",
							"std::shared_ptr<const blockchain::TransactionEntry>",
							"nullptr"
						);
					}
					if (!deferredTransferEntry->getTransactionBody()->isDeferredTransfer()) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr points to a invalid Transaction, wrong type",
							"deferred_transfer_transaction_nr",
							"uint64",
							"DeferredTransfer",
							enum_name(deferredTransferEntry->getTransactionBody()->getTransactionType()).data()
						);
					}
					auto deferredTransfer = deferredTransferEntry->getTransactionBody()->getDeferredTransfer();
					if (!deferredTransfer->getRecipientPublicKey()->isTheSame(mRedeemDeferredTransfer->getSenderPublicKey())) {
						throw TransactionValidationInvalidInputException(
							"deferredTransferTransactionNr points to a invalid Transaction, not matching recipient-sender address pair",
							"deferred_transfer_transaction_nr",
							"uint64",
							deferredTransfer->getRecipientPublicKey()->convertToHex().data(),
							mRedeemDeferredTransfer->getSenderPublicKey()->convertToHex().data()
						);
					}
				}
				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					if (!senderPreviousConfirmedTransaction) {
						throw BlockchainOrderException("deferred transfer transaction not allowed as first transaction on sender blockchain");
					}
					assert(blockchain);
					blockchain::FilterBuilder filterBuilder;

					// check if sender address is deferred transfer
					auto senderAddressType = blockchain->getAddressType(
						filterBuilder
						.setInvolvedPublicKey(mRedeemDeferredTransfer->getSenderPublicKey())
						.setMaxTransactionNr(senderPreviousConfirmedTransaction->getId())
						.build()
					);
					if (data::AddressType::NONE == senderAddressType) {
						throw WrongAddressTypeException(
							"sender address not registered",
							senderAddressType,
							mRedeemDeferredTransfer->getSenderPublicKey()
						);
					}
					else if (data::AddressType::DEFERRED_TRANSFER != senderAddressType) {
						throw WrongAddressTypeException(
							"sender address isn't a deferred transfer",
							senderAddressType,
							mRedeemDeferredTransfer->getSenderPublicKey()
						);
					}
					// check if recipient address was registered
					auto recipientAddressType = blockchain->getAddressType(
						filterBuilder
						.setInvolvedPublicKey(mRedeemDeferredTransfer->getRecipientPublicKey())
						.setMaxTransactionNr(senderPreviousConfirmedTransaction->getId())
						.build()
					);
					if (data::AddressType::NONE == recipientAddressType) {
						throw WrongAddressTypeException(
							"recipient address not registered",
							recipientAddressType, 
							mRedeemDeferredTransfer->getRecipientPublicKey()
						);
					}
					if (data::AddressType::DEFERRED_TRANSFER == recipientAddressType) {
						throw WrongAddressTypeException(
							"recipient cannot be a deferred transfer address", 
							recipientAddressType, 
							mRedeemDeferredTransfer->getRecipientPublicKey()
						);
					}
				}

				// make copy from GradidoTransfer
				auto transfer = std::make_shared<data::GradidoTransfer>(mRedeemDeferredTransfer->getTransfer());
				GradidoTransferRole transferRole(transfer, "");
				transferRole.setConfirmedAt(mConfirmedAt);
				transferRole.setCreatedAt(mCreatedAt);
				// transfer check without account check, account block differ to much
				auto modifiedType = type;
				if ((modifiedType & Type::ACCOUNT) == Type::ACCOUNT) {
					modifiedType = modifiedType - Type::ACCOUNT;
				}
				transferRole.run(modifiedType, blockchain, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);
			}

		}
	}
}