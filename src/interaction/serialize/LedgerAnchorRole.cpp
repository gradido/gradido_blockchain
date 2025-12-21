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
				else if (mLedgerAnchor.isLegacyGradidoDbTransactionId()) {
					message["legacy_transaction_id"_f] = mLedgerAnchor.getLegacyTransactionId();
				}
				else if (mLedgerAnchor.isNodeTriggeredTransactionId()) {
					message["node_trigger_transaction_id"_f] = mLedgerAnchor.getNodeTriggeredTransactionId();
				}
				else if (mLedgerAnchor.isIotaMessageId()) {
					message["iota_message_id"_f] = mLedgerAnchor.getIotaMessageId().copyAsVector();
				}
				else if (mLedgerAnchor.isLegacyGradidoDbCommunityId()) {
					message["legacy_community_id"_f] = mLedgerAnchor.getLegacyTransactionId();
				}
				else if (mLedgerAnchor.isLegacyGradidoDbUserId()) {
					message["legacy_user_id"_f] = mLedgerAnchor.getLegacyTransactionId();
				}
				else if (mLedgerAnchor.isLegacyGradidoDbContributionId()) {
					message["legacy_contribution_id"_f] = mLedgerAnchor.getLegacyTransactionId();
				} 
				else if (mLedgerAnchor.isLegacyGradidoDbTransactionLinkId()) {
					message["legacy_transaction_link_id"_f] = mLedgerAnchor.getLegacyTransactionId();
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
				else if (ledgerAnchor.isIotaMessageId()) {
					sum += 66;
				}
				else if (ledgerAnchor.isLegacyGradidoDbTransactionId()) {
					sum += AbstractRole::serializedSize(ledgerAnchor.getLegacyTransactionId());
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