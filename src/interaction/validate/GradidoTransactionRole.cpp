﻿#include "gradido_blockchain/blockchain/Abstract.h"
#include "gradido_blockchain/blockchain/AbstractProvider.h"
#include "gradido_blockchain/blockchain/Exceptions.h"
#include "gradido_blockchain/blockchain/TransactionEntry.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"
#include "gradido_blockchain/interaction/validate/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/validate/TransactionBodyRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace validate {

			void GradidoTransactionRole::run(
				Type type,
				std::shared_ptr<blockchain::Abstract> blockchain,
				std::shared_ptr<const data::ConfirmedTransaction> senderPreviousConfirmedTransaction,
				std::shared_ptr<const data::ConfirmedTransaction> recipientPreviousConfirmedTransaction
			) {
				const auto& body = mGradidoTransaction.getTransactionBody();
				TransactionBodyRole bodyRole(*body);
				bodyRole.setConfirmedAt(mConfirmedAt);
				// recursive validation					
				bodyRole.run(type, blockchain, senderPreviousConfirmedTransaction, recipientPreviousConfirmedTransaction);

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
					
					std::shared_ptr<const blockchain::TransactionEntry> pairTransactionEntry;
					switch (body->getType()) {
					case data::CrossGroupType::LOCAL: break; // no cross group
					case data::CrossGroupType::INBOUND: break; // happen before OUTBOUND, can only be checked after both transactions are written to blockchain
					case data::CrossGroupType::OUTBOUND:
					case data::CrossGroupType::CROSS:
						if (!mGradidoTransaction.getParingMessageId()) {
							throw TransactionValidationInvalidInputException(
								"parent message id not set for outbound or cross",
								"parent_message_id",
								"bytes[32]"
							);
						}
						else {
							pairTransactionEntry = otherBlockchain->findByMessageId(mGradidoTransaction.getParingMessageId());
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