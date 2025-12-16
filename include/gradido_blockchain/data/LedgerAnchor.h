#ifndef __GRADIDO_BLOCKCHAIN_DATA_LEDGER_ANCHOR_H
#define __GRADIDO_BLOCKCHAIN_DATA_LEDGER_ANCHOR_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"

#include <variant>

namespace gradido {
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT LedgerAnchor
		{
		public:
			enum class Type : uint8_t {
				UNSPECIFIED = 0,
				IOTA_MESSAGE_ID = 1,
				HIERO_TRANSACTION_ID = 2,
				LEGACY_GRADIDO_DB_TRANSACTION_ID = 3,
				NODE_TRIGGER_TRANSACTION_ID = 4
			};
			LedgerAnchor();
			explicit LedgerAnchor(const memory::Block& iotaMessageId);
			explicit LedgerAnchor(const hiero::TransactionId& hieroTransactionId);
			explicit LedgerAnchor(uint64_t transactionId, Type type);
			~LedgerAnchor();

			inline bool isIotaMessageId() const noexcept { return std::holds_alternative<memory::Block>(mValue); }
			inline bool isHieroTransactionId() const noexcept { return std::holds_alternative<hiero::TransactionId>(mValue); }
			inline bool isLegacyGradidoDbTransactionId() const noexcept { return Type::LEGACY_GRADIDO_DB_TRANSACTION_ID == mType; }
			inline bool isNodeTriggeredTransactionId() const noexcept { return Type::NODE_TRIGGER_TRANSACTION_ID == mType; }
			inline Type getType() const noexcept { return mType; }

			inline const memory::Block& getIotaMessageId() const { return std::get<memory::Block>(mValue); }
			inline const hiero::TransactionId& getHieroTransactionId() const { return std::get<hiero::TransactionId>(mValue); }
			inline uint64_t getLegacyTransactionId() const { return std::get<3>(mValue); }
			inline uint64_t getNodeTriggeredTransactionId() const { return std::get<4>(mValue); }

			inline bool isTheSame(const LedgerAnchor& other) const { return mValue == other.mValue; }
			std::string toString() const;
			inline bool empty() const { return Type::UNSPECIFIED == mType || std::holds_alternative<std::monostate>(mValue); }

			inline bool operator==(const LedgerAnchor& other) const { return isTheSame(other); };

		protected:
			using AnchorValue = std::variant<
				std::monostate,
				memory::Block,
				hiero::TransactionId,
				uint64_t,
				uint64_t
			>;
			Type mType;						
			AnchorValue mValue;
		};
	}
}

namespace std {
	template <>
	struct hash<gradido::data::LedgerAnchor> {
		std::size_t operator()(const gradido::data::LedgerAnchor& ledgerAnchor) const noexcept {
			if (ledgerAnchor.isHieroTransactionId()) {
				return hash<hiero::TransactionId>()(ledgerAnchor.getHieroTransactionId());
			} else if (ledgerAnchor.isLegacyGradidoDbTransactionId()) {
				return ledgerAnchor.getLegacyTransactionId();
			} else if (ledgerAnchor.isNodeTriggeredTransactionId()) {
				return ledgerAnchor.getNodeTriggeredTransactionId();
			} else if (ledgerAnchor.isIotaMessageId()) {
				return ledgerAnchor.getIotaMessageId().hash().octet;
			} else {
				return 0;
			}
		}
	};
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_LEDGER_ANCHOR_H