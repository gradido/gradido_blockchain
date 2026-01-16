#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include "magic_enum/magic_enum.hpp"

#include <memory>

using namespace magic_enum;
using std::shared_ptr;

namespace gradido {
	using blockchain::TransactionEntry;
	using data::ConfirmedTransaction, data::CrossGroupType;

	namespace interaction {
		namespace validate {

			void GradidoTransactionRole::run(
				Type type,
				shared_ptr<blockchain::Abstract> blockchain,
				shared_ptr<const ConfirmedTransaction> ownBlockchainPreviousConfirmedTransaction,
				shared_ptr<const ConfirmedTransaction> otherBlockchainPreviousConfirmedTransaction
			) {
				const auto& body = mGradidoTransaction.getTransactionBody();
				TransactionBodyRole bodyRole(*body);
				bodyRole.setConfirmedAt(mConfirmedAt);
				// recursive validation					
				bodyRole.run(type, blockchain, ownBlockchainPreviousConfirmedTransaction, otherBlockchainPreviousConfirmedTransaction);

				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					for (auto& sigPair : mGradidoTransaction.getSignatureMap().getSignaturePairs()) {
						validateEd25519PublicKey(sigPair.getPublicKey(), __FUNCTION__);
						validateEd25519Signature(sigPair.getSignature(), __FUNCTION__);
							
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
				// check signatures
				bodyRole.checkRequiredSignatures(mGradidoTransaction.getSignatureMap(), blockchain);

				if ((type & Type::PAIRED) == Type::PAIRED && !body->getOtherGroup().empty()) {
					assert(blockchain);
					auto otherBlockchain = findBlockchain(blockchain->getProvider(), body->getOtherGroup(), __FUNCTION__);
					
					shared_ptr<const TransactionEntry> pairTransactionEntry;
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
							pairTransactionEntry = otherBlockchain->findByLedgerAnchor(mGradidoTransaction.getPairingLedgerAnchor());
							if (!pairTransactionEntry) {
								throw TransactionValidationException("pairing transaction not found");
							}
							const auto& pairingTransaction = pairTransactionEntry->getConfirmedTransaction()->getGradidoTransaction();
							if(!mGradidoTransaction.isPairing(*pairingTransaction)) {
								throw PairingTransactionNotMatchException(
									"pairing transaction not matching",
									mGradidoTransaction.getSerializedTransaction(),
									pairingTransaction->getSerializedTransaction()
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