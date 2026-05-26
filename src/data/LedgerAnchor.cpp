#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain_core/types/ledger_anchor.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;
using std::shared_ptr, std::to_string;
using memory::ConstBlockPtr;
using hiero::TransactionId;

namespace gradido {
	namespace data {

		LedgerAnchor::LedgerAnchor()
			: mType(GRDT_LEDGER_ANCHOR_UNSPECIFIED)
		{

		}

		LedgerAnchor::LedgerAnchor(const LedgerAnchor& other)
			: mType(other.mType), mValue(other.mValue)
		{

		}

		LedgerAnchor::LedgerAnchor(const TransactionId& hieroTransactionId)
			: mType(GRDT_LEDGER_ANCHOR_HIERO_TRANSACTION_ID), mValue(adapter::toCompact(hieroTransactionId))
		{

		}

		LedgerAnchor::LedgerAnchor(const compact::HieroTransactionId& hieroTransactionId)
			: mType(GRDT_LEDGER_ANCHOR_HIERO_TRANSACTION_ID), mValue(hieroTransactionId)
		{

		}

		LedgerAnchor::LedgerAnchor(uint64_t transactionId, grdt_ledger_anchor type)
			: mType(type)
		{
			if (GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID == type ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_COMMUNITY_ID == type ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_USER_ID == type ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_CONTRIBUTION_ID == type ||
				GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID == type) {
				mValue = AnchorValue(std::in_place_index<2>, transactionId);
			}
			else if (GRDT_LEDGER_ANCHOR_NODE_TRIGGER_TRANSACTION_ID == type) {
				mValue = AnchorValue(std::in_place_index<3>, transactionId);
			}
			else {
				throw GradidoInvalidEnumException(
					"for uint64_t transactionId only types LEGACY_GRADIDO_DB_* and NODE_TRIGGER_TRANSACTION_ID are allowed",
					enum_name(type).data()
				);
			}
		}

		LedgerAnchor::~LedgerAnchor()
		{

		}

		std::string LedgerAnchor::toString() const
		{
			if (isHieroTransactionId()) 
			{
				const auto& data = getHieroTransactionId();
				if (data.empty()) {
					LOG_F(WARNING, "empty hiero transaction id in ledger anchor");
					return "empty hiero transaction id";
				}
				return data.toString();
			} 
			else if (isLegacyGradidoDbTransactionId()) 
			{
				const auto& data = getLegacyGradidoDbId();
				if (!data) {
					LOG_F(WARNING, "empty legacy transaction id in ledger anchor");
					return "empty legacy transaction id";
				}
				return to_string(getLegacyGradidoDbId());
			} 
			else if (isNodeTriggeredTransactionId()) 
			{
				const auto& data = getNodeTriggeredTransactionId();
				if (!data) {
					LOG_F(WARNING, "empty node triggered transaction id in ledger anchor");
					return "empty node triggered transaction id";
				}
				return to_string(getNodeTriggeredTransactionId());
			}
			return "empty";
		}

	}
}