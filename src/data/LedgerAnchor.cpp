#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;
using std::shared_ptr, std::to_string;
using memory::ConstBlockPtr;
using hiero::TransactionId;

namespace gradido {
	namespace data {

		LedgerAnchor::LedgerAnchor()
			: mType(Type::UNSPECIFIED)
		{

		}

		LedgerAnchor::LedgerAnchor(const LedgerAnchor& other)
			: mType(other.mType), mValue(other.mValue)
		{

		}

		LedgerAnchor::LedgerAnchor(const memory::Block& iotaMessageId)
			: mType(Type::IOTA_MESSAGE_ID), mValue(iotaMessageId)
		{

		}

		LedgerAnchor::LedgerAnchor(const TransactionId& hieroTransactionId)
			: mType(Type::HIERO_TRANSACTION_ID), mValue(hieroTransactionId)
		{

		}

		LedgerAnchor::LedgerAnchor(uint64_t transactionId, Type type)
			: mType(type)
		{
			if (Type::LEGACY_GRADIDO_DB_TRANSACTION_ID == type ||
				Type::LEGACY_GRADIDO_DB_COMMUNITY_ID == type ||
				Type::LEGACY_GRADIDO_DB_USER_ID == type ||
				Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID == type ||
				Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID == type) {
				mValue = AnchorValue(std::in_place_index<3>, transactionId);
			}
			else if (Type::NODE_TRIGGER_TRANSACTION_ID == type) {
				mValue = AnchorValue(std::in_place_index<4>, transactionId);
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
			if (isIotaMessageId()) 
			{
				const auto& data = getIotaMessageId();
				if (data.isEmpty()) {
					LOG_F(WARNING, "empty iota message id in ledger anchor");
					return "empty iota message id";
				}
				return data.copyAsString();
			} 
			else if (isHieroTransactionId()) 
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
				const auto& data = getLegacyTransactionId();
				if (!data) {
					LOG_F(WARNING, "empty legacy transaction id in ledger anchor");
					return "empty legacy transaction id";
				}
				return to_string(getLegacyTransactionId());
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