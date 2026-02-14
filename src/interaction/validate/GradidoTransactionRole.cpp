#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/blockchain/Filter.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <memory>

using namespace magic_enum;
using std::shared_ptr, std::make_shared;

namespace gradido {
	using blockchain::Filter, blockchain::TransactionEntry;
	using data::ConfirmedTransaction, data::CrossGroupType, data::GradidoTransaction;

	namespace interaction {
		namespace validate {

			void GradidoTransactionRole::run(Type type, ContextData& c)
			{
				const auto& body = mGradidoTransaction.getTransactionBody();

				// cross group transaction preparations
				shared_ptr<blockchain::Abstract> otherBlockchain;
				if (body->getOtherCommunityIdIndex().has_value() && c.senderBlockchain) {
					otherBlockchain = findBlockchain(c.senderBlockchain->getProvider(), body->getOtherCommunityIdIndex().value(), __FUNCTION__);
					shared_ptr<const ConfirmedTransaction> otherPreviousTx;
					if (otherBlockchain && !mGradidoTransaction.getPairingLedgerAnchor().empty()) {
						c.pairingTx = otherBlockchain->findByLedgerAnchor(mGradidoTransaction.getPairingLedgerAnchor());
					}
					if (c.pairingTx) {
						auto otherPreviousTxEntry = otherBlockchain->getTransactionForId(c.pairingTx->getTransactionNr());
						if (otherPreviousTxEntry) {
							otherPreviousTx = otherPreviousTxEntry->getConfirmedTransaction();
						}
					}
					if (body->getType() == CrossGroupType::OUTBOUND) {
						c.recipientBlockchain = otherBlockchain;
						if (otherPreviousTx) {
							c.recipientPreviousConfirmedTransaction = otherPreviousTx;
						}

					}
					else if (body->getType() == CrossGroupType::INBOUND)
					{
						c.recipientBlockchain = c.senderBlockchain;
						c.senderBlockchain = otherBlockchain;
						c.recipientPreviousConfirmedTransaction = c.senderPreviousConfirmedTransaction;
						if (otherPreviousTx) {
							c.senderPreviousConfirmedTransaction = otherPreviousTx;
						}
					}
					else {
						LOG_F(WARNING, "CrossGroupType::%s not implemented in GradidoTransactionRole", enum_name(body->getType()).data());
					}
					auto lastRecipientEntry = c.recipientBlockchain->findOne(Filter::LAST_TRANSACTION);
					if (!lastRecipientEntry) {
						throw GradidoNodeInvalidDataException("missing last transaction of other community id");
					}
					c.recipientPreviousConfirmedTransaction = lastRecipientEntry->getConfirmedTransaction();
				}

				TransactionBodyRole bodyRole(*body);
				bodyRole.setConfirmedAt(mConfirmedAt);
				// recursive validation					
				bodyRole.run(type, c);

				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					for (auto& sigPair : mGradidoTransaction.getSignatureMap().getSignaturePairs()) {
						validateEd25519PublicKey(sigPair.getPublicKey(), __FUNCTION__);
						validateEd25519Signature(sigPair.getSignature(), __FUNCTION__);
						if (!mDisableVerify) {
							KeyPairEd25519 key_pair(sigPair.getPublicKey());
							if (!key_pair.verify(*mGradidoTransaction.getBodyBytes(), *sigPair.getSignature())) {
								throw TransactionValidationInvalidSignatureException(
									"pubkey don't belong to body bytes",
									sigPair.getPublicKey(),
									sigPair.getSignature(),
									mGradidoTransaction.getBodyBytes()
								);
							}
						}
					}
				}
				// check signatures
				bodyRole.checkRequiredSignatures(mGradidoTransaction.getSignatureMap(), c.senderBlockchain);

				if ((type & Type::PAIRED) == Type::PAIRED && body->getOtherCommunityIdIndex().has_value()) 
				{
					switch (body->getType()) {
					case CrossGroupType::LOCAL: break; // no cross group
					case CrossGroupType::OUTBOUND: break; // happen first, no pairing transaction yet
					case CrossGroupType::INBOUND:
					case CrossGroupType::CROSS:
						if (mGradidoTransaction.getPairingLedgerAnchor().empty()) {
							throw TransactionValidationInvalidInputException(
								"pairing ledger anchor not set for outbound or cross",
								"pairing_ledger_anchor",
								"LedgerAnchor"
							);
						}
						else {
							if (!c.pairingTx || !c.pairingTx->getConfirmedTransaction()) {
								throw TransactionValidationException("pairing transaction not found or invalid");
							}
							if(!mGradidoTransaction.isPairing(*c.pairingTx->getConfirmedTransaction()->getGradidoTransaction())) {
								throw PairingTransactionNotMatchException(
									"pairing transaction not matching",
									make_shared<const GradidoTransaction>(mGradidoTransaction),
									c.pairingTx->getConfirmedTransaction()->getGradidoTransaction()
								);
							}
						}
						break;
					default: throw GradidoUnknownEnumException("unknown cross group type", "data::CrossGroupType", enum_name(body->getType()).data());
					}
				}				
			}
		}
	}
}