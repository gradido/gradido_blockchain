#include "gradido_blockchain/interaction/serialize/LedgerAnchorRole.h"
#include "gradido_blockchain/interaction/serialize/HieroTransactionIdRole.h"
#include "gradido_blockchain/data/LedgerAnchor.h"

namespace gradido {
	namespace interaction {
		namespace serialize {
			LedgerAnchorMessage LedgerAnchorRole::getMessage() const
			{
				LedgerAnchorMessage message;
				message["type"_f] = mLedgerAnchor.getType();
				if (mLedgerAnchor.isHieroTransactionId()) {
					message["hiero_transaction_id"_f] = HieroTransactionIdRole(mLedgerAnchor.getHieroTransactionId()).getMessage();
				}
				else if (mLedgerAnchor.isLegacyGradidoId()) {
					message["id"_f] = mLedgerAnchor.getLegacyGradidoDbId();
				}
				else if (mLedgerAnchor.isNodeTriggeredTransactionId()) {
					message["id"_f] = mLedgerAnchor.getNodeTriggeredTransactionId();
				}
				return message;
			}

			size_t LedgerAnchorRole::calculateSerializedSize() const
			{
				return serializedSize(mLedgerAnchor);
			}

			size_t LedgerAnchorRole::serializedSize(const data::LedgerAnchor& ledgerAnchor)
			{
				size_t sum = 2;
				if (ledgerAnchor.isHieroTransactionId()) {
					sum += 36;
				}
				else if (ledgerAnchor.isLegacyGradidoDbTransactionId()) {
					sum += AbstractRole::serializedSize(ledgerAnchor.getLegacyGradidoDbId());
				}
				else if (ledgerAnchor.isNodeTriggeredTransactionId()) {
					sum += AbstractRole::serializedSize(ledgerAnchor.getNodeTriggeredTransactionId());
				}
				// + 4 Bytes to have a error marging
				return sum + 4;
			}
		}
	}
}