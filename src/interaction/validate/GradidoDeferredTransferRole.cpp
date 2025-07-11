#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/FilterBuilder.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/tz.h"

namespace gradido {
	namespace interaction {
		namespace validate {

			GradidoDeferredTransferRole::GradidoDeferredTransferRole(std::shared_ptr<const data::GradidoDeferredTransfer> deferredTransfer)
				: mDeferredTransfer(deferredTransfer) 
			{
				assert(deferredTransfer);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(deferredTransfer->getTransfer().getSender().getPublicKey());
			}

			void GradidoDeferredTransferRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mDeferredTransfer->getTimeoutDuration().getAsDuration() > GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL) {
						std::string expected = DataTypeConverter::timespanToString(mDeferredTransfer->getTimeoutDuration().getAsDuration())
						+ " <= " 
						+ DataTypeConverter::timespanToString(GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL);						
						throw TransactionValidationInvalidInputException(
							"timeoutDuration is to long", 
							"timeout_duration", 
							"uint32",
							expected.data(),
							DataTypeConverter::timespanToString(mDeferredTransfer->getTimeoutDuration()).data()
						);
					}
					if (mDeferredTransfer->getTimeoutDuration().getAsDuration() < GRADIDO_DEFERRED_TRANSFER_MIN_TIMEOUT_INTERVAL) {
						std::string expected = DataTypeConverter::timespanToString(mDeferredTransfer->getTimeoutDuration().getAsDuration())
							+ " >= "
							+ DataTypeConverter::timespanToString(GRADIDO_DEFERRED_TRANSFER_MIN_TIMEOUT_INTERVAL);
						throw TransactionValidationInvalidInputException(
							"timeoutDuration is to short",
							"timeout_duration",
							"uint32",
							expected.data(),
							DataTypeConverter::timespanToString(mDeferredTransfer->getTimeoutDuration()).data()
						);
					}
				}
				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					if (!senderPreviousConfirmedTransaction) {
						throw BlockchainOrderException("deferred transfer transaction not allowed as first transaction on sender blockchain");
					}
					assert(blockchain);
					blockchain::FilterBuilder filterBuilder;

					// check if sender address was registered
					auto senderAddressType = blockchain->getAddressType(
						filterBuilder
						.setInvolvedPublicKey(mDeferredTransfer->getSenderPublicKey())
						.setMaxTransactionNr(senderPreviousConfirmedTransaction->getId())
						.build()
					);
					if (data::AddressType::NONE == senderAddressType) {
						throw WrongAddressTypeException(
							"sender address not registered",
							senderAddressType,
							mDeferredTransfer->getSenderPublicKey()
						);
					}
					else if (data::AddressType::DEFERRED_TRANSFER == senderAddressType) {
						throw WrongAddressTypeException(
							"sender address is deferred transfer, please use redeemDeferredTransferTransaction for that",
							senderAddressType,
							mDeferredTransfer->getSenderPublicKey()
						);
					}
					// check if recipient address was registered
					auto recipientAddressType = blockchain->getAddressType(
						filterBuilder
						.setInvolvedPublicKey(mDeferredTransfer->getRecipientPublicKey())
						.setMaxTransactionNr(senderPreviousConfirmedTransaction->getId())
						.build()
					);
					// with deferred transfer recipient address is completely new 
					if (data::AddressType::NONE != recipientAddressType) {
						throw WrongAddressTypeException("deferred transfer address already exist", recipientAddressType, mDeferredTransfer->getRecipientPublicKey());
					}
				}
				// make copy from GradidoTransfer
				auto transfer = std::make_shared<data::GradidoTransfer>(mDeferredTransfer->getTransfer());
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