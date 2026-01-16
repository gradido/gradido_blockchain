#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/validate/InboundGradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include <cassert>
#include <memory>

#include "date/date.h"

using std::shared_ptr;

namespace gradido {
	using blockchain::Filter;
	using data::AddressType, data::ConfirmedTransaction;
	namespace interaction {
		namespace validate {

			void InboundGradidoTransferRole::validateAccount(
				const ConfirmedTransaction& ownBlockchainPreviousConfirmedTransaction,
				const ConfirmedTransaction& otherBlockchainPreviousConfirmedTransaction,
				shared_ptr<blockchain::Abstract> ownBlockchain,
				shared_ptr<blockchain::Abstract> otherBlockchain
			) {
				assert(ownBlockchain);
				assert(otherBlockchain);
				Filter filter;
				filter.involvedPublicKey = mGradidoTransfer->getSender().getPublicKey();
				filter.maxTransactionNr = otherBlockchainPreviousConfirmedTransaction.getId();

				// check if sender address was registered
				auto senderAddressType = otherBlockchain->getAddressType(filter);
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
				filter.maxTransactionNr = ownBlockchainPreviousConfirmedTransaction.getId();
				auto recipientAddressType = ownBlockchain->getAddressType(filter);
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