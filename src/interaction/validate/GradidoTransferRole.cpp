#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/GradidoTransfer.h"
#include "gradido_blockchain/interaction/validate/GradidoTransferRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/interaction/calculateAccountBalance/Context.h"
#include "gradido_blockchain/interaction/validate/TransferAmountRole.h"

#include <cassert>
#include <memory>
#include <string_view>

#include "date/date.h"

using std::shared_ptr;
using std::string_view;

namespace gradido {
	using blockchain::Filter;
	using data::ConfirmedTransaction, data::GradidoTransfer;
	namespace interaction {
		namespace validate {

			GradidoTransferRole::GradidoTransferRole(shared_ptr<const GradidoTransfer> gradidoTransfer)
				: mGradidoTransfer(gradidoTransfer), mCrossGroupType(GRDT_CROSS_GROUP_LOCAL)
			{
				assert(gradidoTransfer);
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeys.push_back(gradidoTransfer->getSender().getPublicKey());
			}

			void GradidoTransferRole::run(Type type, ContextData& c)
			{
				TransferAmountRole transferAmountRole(mGradidoTransfer->getSender());
				transferAmountRole.setCrossGroupType(mCrossGroupType);
				transferAmountRole.run(type, c);
				auto& sender = mGradidoTransfer->getSender();

				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					validateEd25519PublicKey(mGradidoTransfer->getRecipient(), "recipient");

					if (mGradidoTransfer->getRecipient()->isTheSame(sender.getPublicKey())) {
						throw TransactionValidationException("sender and recipient are the same");
					}
				}

				if ((type & Type::ACCOUNT) == Type::ACCOUNT) {
					assert(c.senderBlockchain);
					if (!c.senderPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on sender blockchain");
					}
					if (!c.recipientPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on recipient blockchain");
					}
					validateAccount(c);
				}

				if ((type & Type::PREVIOUS) == Type::PREVIOUS)
				{
					if (!c.senderPreviousConfirmedTransaction) {
						throw BlockchainOrderException("transfer transaction not allowed as first transaction on blockchain");
					}
				}

				if ((type & Type::PREVIOUS_BALANCE) == Type::PREVIOUS_BALANCE)
				{
					validatePrevious(*c.senderPreviousConfirmedTransaction, c.senderBlockchain);
				}
			}

			void GradidoTransferRole::validatePrevious(
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
					sender.getCoinCommunityIdIndex(),
					previousConfirmedTransaction.getId() // calculate until this transaction nr
				);

				if (sender.getAmount() > finalBalance + GradidoUnit::fromGradidoCent(100)) {
					throw InsufficientBalanceException("not enough Gradido Balance for send coins", sender.getAmount(), finalBalance);
				}
			}

			void GradidoTransferRole::validateAccount(ContextData& c)
			{
				assert(c.senderBlockchain);
				assert(c.recipientBlockchain);
				Filter filter;
				filter.involvedPublicKey = mGradidoTransfer->getSender().getPublicKey();
				filter.maxTransactionNr = c.senderPreviousConfirmedTransaction->getId();

				// check if sender address was registered
				auto sendergrdt_address = c.senderBlockchain->getAddressType(filter);
				if (GRDT_ADDRESS_NONE == sendergrdt_address) {
					throw Wronggrdt_addressException(
						"sender address not registered",
						sendergrdt_address,
						mGradidoTransfer->getSender().getPublicKey(),
						c.senderBlockchain->getCommunityIdIndex()
					);
				}
				if (GRDT_ADDRESS_DEFERRED_TRANSFER == sendergrdt_address) {
					throw Wronggrdt_addressException(
						"sender address is deferred transfer, please use redeemDeferredTransferTransaction for that",
						sendergrdt_address,
						mGradidoTransfer->getSender().getPublicKey(),
						c.senderBlockchain->getCommunityIdIndex()
					);
				}

				// check if recipient address was registered
				filter.involvedPublicKey = mGradidoTransfer->getRecipient();
				filter.maxTransactionNr = c.recipientPreviousConfirmedTransaction->getId();
				auto recipientgrdt_address = c.recipientBlockchain->getAddressType(filter);
				if (GRDT_ADDRESS_NONE == recipientgrdt_address) {
					throw Wronggrdt_addressException(
						"recipient address not registered", 
						recipientgrdt_address, 
						mGradidoTransfer->getRecipient(),
						c.recipientBlockchain->getCommunityIdIndex()
					);
				}
				if (GRDT_ADDRESS_DEFERRED_TRANSFER == recipientgrdt_address) {
					throw Wronggrdt_addressException(
						"recipient cannot be a deferred transfer address",
						recipientgrdt_address,
						mGradidoTransfer->getRecipient(),
						c.recipientBlockchain->getCommunityIdIndex()
					);
				}
			}
		}
	}
}