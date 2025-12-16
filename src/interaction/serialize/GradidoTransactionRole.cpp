#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"
#include "gradido_blockchain/interaction/serialize/LedgerAnchorRole.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

			GradidoTransactionRole::GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction)
				: mGradidoTransaction(gradidoTransaction), mSigantureMapRole(gradidoTransaction.getSignatureMap())
			{
			}

			GradidoTransactionMessage GradidoTransactionRole::getMessage() const
			{
				GradidoTransactionMessage gradidoTransactionMessage;
					
				if (mGradidoTransaction.getBodyBytes()) {
					gradidoTransactionMessage["body_bytes"_f] = mGradidoTransaction.getBodyBytes()->copyAsVector();
				}
				LedgerAnchorRole ledgerAnchorRole(mGradidoTransaction.getPairingLedgerAnchor());
				gradidoTransactionMessage["pairing_ledger_anchor"_f] = ledgerAnchorRole.getMessage();
				gradidoTransactionMessage["sig_map"_f] = mSigantureMapRole.getMessage();
				return gradidoTransactionMessage;
			}

			size_t GradidoTransactionRole::calculateSerializedSize() const
			{
				size_t size = mSigantureMapRole.calculateSerializedSize();
				if (mGradidoTransaction.getBodyBytes()) {
					size += mGradidoTransaction.getBodyBytes()->size();
				}

				size += LedgerAnchorRole::serializedSize(mGradidoTransaction.getPairingLedgerAnchor());
				return size;
			}

		}
	}
}

