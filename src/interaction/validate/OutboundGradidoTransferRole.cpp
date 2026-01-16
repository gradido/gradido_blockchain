#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/GradidoTransfer.h"
#include "gradido_blockchain/interaction/validate/OutboundGradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include <cassert>
#include <memory>
#include <string_view>

#include "date/date.h"

using std::shared_ptr;
using std::string_view;

namespace gradido {
	using blockchain::Filter;
	using data::AddressType, data::ConfirmedTransaction, data::GradidoTransfer;
	namespace interaction {
		namespace validate {

			OutboundGradidoTransferRole::OutboundGradidoTransferRole(shared_ptr<const GradidoTransfer> gradidoTransfer, string_view otherCommunity)
				: LocalGradidoTransferRole(gradidoTransfer), mOtherCommunity(otherCommunity)
			{
			}

			void OutboundGradidoTransferRole::run(
				Type type,
				shared_ptr<blockchain::Abstract> blockchain,
				shared_ptr<const ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
				shared_ptr<const ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
			) {
				Type typeCopy = type;
				if (hasFlag(typeCopy, Type::ACCOUNT)) {
					typeCopy = typeCopy - Type::ACCOUNT;
				}
				LocalGradidoTransferRole::run(
					typeCopy,
					blockchain,
					ownBlockchainPreviousConfirmedTransaction,
					otherBlockchainPreviousConfirmedTransaction
				);

				if (hasFlag(type, Type::ACCOUNT)) {
					assert(blockchain);
					std::shared_ptr<blockchain::Abstract> recipientBlockchain;
					if (mOtherCommunity.empty()) {
						throw TransactionValidationException("missing other community for Cross Group Transfer: Outbound");
					}
					recipientBlockchain = findBlockchain(blockchain->getProvider(), mOtherCommunity, __FUNCTION__);
					
					if (!ownBlockchainPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on blockchain");
					}
					if (!otherBlockchainPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on other blockchain");
					}
					validateAccount(
						*ownBlockchainPreviousConfirmedTransaction,
						*otherBlockchainPreviousConfirmedTransaction,
						blockchain,
						recipientBlockchain
					);
				}
			}

			void OutboundGradidoTransferRole::validateAccount(
				const ConfirmedTransaction& ownBlockchainPreviousConfirmedTransaction,
				const ConfirmedTransaction& otherBlockchainPreviousConfirmedTransaction,
				shared_ptr<blockchain::Abstract> ownBlockchain,
				shared_ptr<blockchain::Abstract> otherBlockchain
			) {
				assert(ownBlockchain);
				assert(otherBlockchain);
				Filter filter;
				filter.involvedPublicKey = mGradidoTransfer->getSender().getPublicKey();
				filter.maxTransactionNr = ownBlockchainPreviousConfirmedTransaction.getId();

				// check if sender address was registered
				auto senderAddressType = ownBlockchain->getAddressType(filter);
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
				filter.maxTransactionNr = otherBlockchainPreviousConfirmedTransaction.getId();
				auto recipientAddressType = otherBlockchain->getAddressType(filter);
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