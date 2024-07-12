#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {

			void GradidoTransactionRole::run(
				Type type,
				std::string_view communityId,
				blockchain::AbstractProvider* blockchainProvider,
				data::ConstConfirmedTransactionPtr senderPreviousConfirmedTransaction,
				data::ConstConfirmedTransactionPtr recipientPreviousConfirmedTransaction
			) {
				const auto& body = mGradidoTransaction.getTransactionBody();
				TransactionBodyRole bodyRole(*body);
				bodyRole.setConfirmedAt(mConfirmedAt);
				// recursive validation					
				bodyRole.run(type, communityId, blockchainProvider, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);

				if ((type & Type::SINGLE) == Type::SINGLE)
				{
					for (auto& sigPair : mGradidoTransaction.signatureMap.signaturePairs) {
						validateEd25519PublicKey(sigPair.pubkey, __FUNCTION__);
						validateEd25519Signature(sigPair.signature, __FUNCTION__);
							
						KeyPairEd25519 key_pair(sigPair.pubkey);
						if (!key_pair.verify(*mGradidoTransaction.bodyBytes, *sigPair.signature)) {
							throw TransactionValidationInvalidSignatureException(
								"pubkey don't belong to body bytes", 
								sigPair.pubkey,
								sigPair.signature, 
								mGradidoTransaction.bodyBytes
							);
						}
					}
				}
				// check signatures
				std::shared_ptr<blockchain::Abstract> blockchain;
				if (blockchainProvider && !communityId.empty()) {
					blockchain = blockchainProvider->findBlockchain(communityId);
				}
				bodyRole.checkRequiredSignatures(mGradidoTransaction.signatureMap, blockchain);				

				if ((type & Type::PAIRED) == Type::PAIRED) {
					assert(blockchainProvider);
					auto otherBlockchain = blockchainProvider->findBlockchain(body->otherGroup);
					assert(otherBlockchain);

					std::shared_ptr<blockchain::TransactionEntry> pairTransactionEntry;
					switch (body->type) {
					case data::CrossGroupType::LOCAL: break; // no cross group
					case data::CrossGroupType::INBOUND: break; // happen before OUTBOUND, can only be checked after both transactions are written to blockchain
					case data::CrossGroupType::OUTBOUND:
					case data::CrossGroupType::CROSS:
						if (!mGradidoTransaction.paringMessageId) {
							throw TransactionValidationInvalidInputException("paring message id not set for outbound", "paring message id", "binary");
						}
						else {
							pairTransactionEntry = otherBlockchain->findByMessageId(mGradidoTransaction.paringMessageId);
							if (!pairTransactionEntry) {
								throw TransactionValidationException("pairing transaction not found");
							}
							const auto& pairingTransaction = pairTransactionEntry->getConfirmedTransaction()->gradidoTransaction;
							if(!mGradidoTransaction.isPairing(*pairingTransaction)) {
								throw PairingTransactionNotMatchException(
									"pairing transaction not matching",
									mGradidoTransaction.getSerializedTransaction(),
									pairingTransaction->getSerializedTransaction()
								);
							}
						}
						break;
					default: throw GradidoUnknownEnumException("unknown cross group type", "data::CrossGroupType", enum_name(body->type).data());
					}
				}				
			}
		}
	}
}