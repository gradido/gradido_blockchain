#ifndef __GRADIDO_BLOCKCHAIN_DATA_LEDGER_ANCHOR_H
#define __GRADIDO_BLOCKCHAIN_DATA_LEDGER_ANCHOR_H

#include "gradido_blockchain/crypto/SignatureOctet.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/data/adapter/hiero.h"
#include "gradido_blockchain/data/compact/HieroTransactionId.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain_core/types/ledger_anchor.h"

#include <variant>

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT LedgerAnchor
		{
		public:

			LedgerAnchor();
			LedgerAnchor(const LedgerAnchor& other);
			explicit LedgerAnchor(const hiero::TransactionId& hieroTransactionId);
			explicit LedgerAnchor(const compact::HieroTransactionId& hieroTransactionId);
			explicit LedgerAnchor(uint64_t transactionId, grdt_ledger_anchor type);
			~LedgerAnchor();

			inline bool isHieroTransactionId() const noexcept { return std::holds_alternative<compact::HieroTransactionId>(mValue); }
			inline bool isLegacyGradidoDbTransactionId() const noexcept { return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID == mType; }
			inline bool isNodeTriggeredTransactionId() const noexcept { return GRDT_LEDGER_ANCHOR_NODE_TRIGGER_TRANSACTION_ID == mType; }
			inline bool isLegacyGradidoDbCommunityId() const noexcept { return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_COMMUNITY_ID == mType; }
			inline bool isLegacyGradidoDbUserId() const noexcept { return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_USER_ID == mType; }
			inline bool isLegacyGradidoDbContributionId() const noexcept { return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_CONTRIBUTION_ID == mType; }
			inline bool isLegacyGradidoDbTransactionLinkId() const noexcept { return GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID == mType; }
			inline bool isLegacyGradidoId() const noexcept;
			inline grdt_ledger_anchor getType() const noexcept { return mType; }

			inline const hiero::TransactionId getHieroTransactionId() const { return adapter::fromCompact(std::get<compact::HieroTransactionId>(mValue)); }
			inline const compact::HieroTransactionId getHieroTransactionIdCompact() const { return std::get<compact::HieroTransactionId>(mValue); }
			// return id, valid for all Legacy Gradido DB Types
			inline uint64_t getLegacyGradidoDbId() const { return std::get<2>(mValue); }
			inline uint64_t getNodeTriggeredTransactionId() const { return std::get<3>(mValue); }

			inline bool isTheSame(const LedgerAnchor& other) const { return mType == other.mType && mValue == other.mValue; }
			std::string toString() const;
			inline bool empty() const { return GRDT_LEDGER_ANCHOR_UNSPECIFIED == mType || std::holds_alternative<std::monostate>(mValue); }

			inline bool operator==(const LedgerAnchor& other) const { return isTheSame(other); };

		protected:
			using AnchorValue = std::variant<
				std::monostate,
				compact::HieroTransactionId,
				uint64_t,
				uint64_t
			>;
			grdt_ledger_anchor mType;
			AnchorValue mValue;
		};

		bool LedgerAnchor::isLegacyGradidoId() const noexcept
		{
			return
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID == mType ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_COMMUNITY_ID == mType ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_USER_ID == mType ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_CONTRIBUTION_ID == mType ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID == mType;
		}
	}
}

namespace std {
	template <>
	struct hash<gradido::data::LedgerAnchor> {
		std::size_t operator()(const gradido::data::LedgerAnchor& ledgerAnchor) const noexcept {
			if (ledgerAnchor.isHieroTransactionId()) {
				return hash<gradido::data::compact::HieroTransactionId>()(ledgerAnchor.getHieroTransactionIdCompact());
			}
			else if (ledgerAnchor.isNodeTriggeredTransactionId()) {
				return ledgerAnchor.getNodeTriggeredTransactionId();
			}
			else if (ledgerAnchor.empty()) {
				return 0;
			}
			else {
				return ledgerAnchor.getLegacyGradidoDbId();
			}
		}
	};
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_LEDGER_ANCHOR_H