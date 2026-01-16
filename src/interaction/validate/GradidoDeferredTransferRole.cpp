#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/interaction/validate/GradidoDeferredTransferRole.h"
#include "gradido_blockchain/interaction/validate/LocalGradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "date/date.h"

#include <memory>
#include <string>

using DataTypeConverter::timespanToString;
using std::shared_ptr;
using std::string;

namespace gradido {
	using blockchain::Filter;
	using data::AddressType, data::ConfirmedTransaction, data::GradidoDeferredTransfer;
	namespace interaction {
		namespace validate {

			GradidoDeferredTransferRole::GradidoDeferredTransferRole(shared_ptr<const GradidoDeferredTransfer> deferredTransfer)
				: mDeferredTransfer(deferredTransfer) 
			{
				assert(deferredTransfer);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(deferredTransfer->getTransfer().getSender().getPublicKey());
			}

			void GradidoDeferredTransferRole::run(
				Type type,
				shared_ptr<blockchain::Abstract> blockchain,
				shared_ptr<const ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
				shared_ptr<const ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
			) {
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (mDeferredTransfer->getTimeoutDuration().getAsDuration() > GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL) {
						string expected = timespanToString(mDeferredTransfer->getTimeoutDuration().getAsDuration())
						+ " <= " 
						+ timespanToString(GRADIDO_DEFERRED_TRANSFER_MAX_TIMEOUT_INTERVAL);						
						throw TransactionValidationInvalidInputException(
							"timeoutDuration is to long", 
							"timeout_duration", 
							"uint32",
							expected.data(),
							timespanToString(mDeferredTransfer->getTimeoutDuration()).data()
						);
					}
					if (mDeferredTransfer->getTimeoutDuration().getAsDuration() < GRADIDO_DEFERRED_TRANSFER_MIN_TIMEOUT_INTERVAL) {
						string expected = timespanToString(mDeferredTransfer->getTimeoutDuration().getAsDuration())
							+ " >= "
							+ timespanToString(GRADIDO_DEFERRED_TRANSFER_MIN_TIMEOUT_INTERVAL);
						throw TransactionValidationInvalidInputException(
							"timeoutDuration is to short",
							"timeout_duration",
							"uint32",
							expected.data(),
							timespanToString(mDeferredTransfer->getTimeoutDuration()).data()
						);
					}
				}
				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					if (!ownBlockchainPreviousConfirmedTransaction) {
						throw BlockchainOrderException("deferred transfer transaction not allowed as first transaction on blockchain");
					}
					assert(blockchain);
					Filter filter(Filter::LAST_TRANSACTION);
					filter.involvedPublicKey = mDeferredTransfer->getSenderPublicKey();
					filter.maxTransactionNr = ownBlockchainPreviousConfirmedTransaction->getId();

					// check if sender address was registered
					auto senderAddressType = blockchain->getAddressType(filter);
					if (AddressType::NONE == senderAddressType) {
						throw WrongAddressTypeException(
							"sender address not registered",
							senderAddressType,
							mDeferredTransfer->getSenderPublicKey()
						);
					}
					else if (AddressType::DEFERRED_TRANSFER == senderAddressType) {
						throw WrongAddressTypeException(
							"sender address is deferred transfer, please use redeemDeferredTransferTransaction for that",
							senderAddressType,
							mDeferredTransfer->getSenderPublicKey()
						);
					}
					// check if recipient address was registered
					filter.involvedPublicKey = mDeferredTransfer->getRecipientPublicKey();
					auto recipientAddressType = blockchain->getAddressType(filter);
					// with deferred transfer recipient address is completely new 
					if (AddressType::NONE != recipientAddressType) {
						throw WrongAddressTypeException("deferred transfer address already exist", recipientAddressType, mDeferredTransfer->getRecipientPublicKey());
					}
				}
				// make copy from GradidoTransfer
				auto transfer = std::make_shared<data::GradidoTransfer>(mDeferredTransfer->getTransfer());
				LocalGradidoTransferRole transferRole(transfer);
				transferRole.setConfirmedAt(mConfirmedAt);
				transferRole.setCreatedAt(mCreatedAt);
				// transfer check without account check, account block differ to much
				auto modifiedType = type;
				if ((modifiedType & Type::ACCOUNT) == Type::ACCOUNT) {
					modifiedType = modifiedType - Type::ACCOUNT;
				}
				transferRole.run(modifiedType, blockchain, ownBlockchainPreviousConfirmedTransaction, otherBlockchainPreviousConfirmedTransaction);
			}

		}
	}
}