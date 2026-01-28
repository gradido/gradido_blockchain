#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/data/CommunityFriendsUpdate.h"
#include "gradido_blockchain/data/CommunityRoot.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/data/DurationSeconds.h"
#include "gradido_blockchain/data/EncryptedMemo.h"
#include "gradido_blockchain/data/GradidoCreation.h"
#include "gradido_blockchain/data/GradidoDeferredTransfer.h"
#include "gradido_blockchain/data/GradidoRedeemDeferredTransfer.h"
#include "gradido_blockchain/data/GradidoTimeoutDeferredTransfer.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/data/GradidoTransfer.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/RegisterAddress.h"
#include "gradido_blockchain/data/SignatureMap.h"
#include "gradido_blockchain/data/SignaturePair.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TimestampSeconds.h"
#include "gradido_blockchain/data/TransferAmount.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/lib/DictionaryExceptions.h"
#include "gradido_blockchain/serialization/toJson.h"

#include "magic_enum/magic_enum.hpp"
#include <string>

using namespace rapidjson;
using namespace gradido::data;
using gradido::g_appContext;
using std::to_string;

namespace serialization {

	// basic 
	DEFINE_TO_JSON(TransferAmount, {
		obj.AddMember("pubkey", toJson(value.getPublicKey(), alloc), alloc);
		obj.AddMember("amount", toJson(value.getAmount(), alloc), alloc);
		auto communityIdOptional = g_appContext->getCommunityIds().getDataForIndex(value.getCoinCommunityIdIndex());
		if (communityIdOptional.has_value()) {
			obj.AddMember("coinCommunityId", toJson(communityIdOptional.value(), alloc), alloc);
		}
	})

	DEFINE_TO_JSON(TransactionTriggerEvent, {
		obj.AddMember("linkedTransactionNr", value.getLinkedTransactionId(), alloc);
		obj.AddMember("targetDate", toJson(value.getTargetDate().getAsTimepoint(), alloc), alloc);
		obj.AddMember("type", toJson(value.getType(), alloc), alloc);
	})

	DEFINE_TO_JSON(AccountBalance, {
		obj.AddMember("pubkey", toJson(value.getPublicKey(), alloc), alloc);
		obj.AddMember("balance", toJson(value.getBalance(), alloc), alloc);
		auto communityIdOptional = g_appContext->getCommunityIds().getDataForIndex(value.getCoinCommunityIdIndex());
		if (communityIdOptional.has_value()) {
			obj.AddMember("coinCommunityId", toJson(communityIdOptional.value(), alloc), alloc);
		}
	})

	DEFINE_TO_JSON(DurationSeconds, {
		obj.AddMember("seconds", value.getSeconds(), alloc);
	})

	DEFINE_TO_JSON(EncryptedMemo, {
		obj.AddMember("type", toJson(value.getKeyType(), alloc), alloc);
		if (MemoKeyType::PLAIN == value.getKeyType()) {
			obj.AddMember("memo", toJson(value.getMemo().copyAsString(), alloc), alloc);
		}
		else {
			obj.AddMember("memo", toJson(value.getMemo().convertToBase64(), alloc), alloc);
		}
	})

	DEFINE_TO_JSON(SignatureMap, {
		obj.AddMember("signatureMap", toJson(value.getSignaturePairs(), alloc), alloc);
	})

	DEFINE_TO_JSON(SignaturePair, {
		obj.AddMember("pubkey", toJson(value.getPublicKey(), alloc), alloc);
		obj.AddMember("signature", toJson(value.getSignature(), alloc), alloc);
	})

	DEFINE_TO_JSON(Timestamp, {
		obj.AddMember("seconds", value.getSeconds(), alloc);
		obj.AddMember("nano", value.getNanos(), alloc);
	})

	DEFINE_TO_JSON(TimestampSeconds, {
		obj.AddMember("seconds", value.getSeconds(), alloc);
	})

	// transaction type

	DEFINE_TO_JSON(CommunityFriendsUpdate, {
		obj.AddMember("colorFusion", value.getColorFusion(), alloc);
	})

	DEFINE_TO_JSON(CommunityRoot, {
		obj.AddMember("pubkey", toJson(value.getPublicKey(), alloc), alloc);
		obj.AddMember("gmwPubkey", toJson(value.getGmwPubkey(), alloc), alloc);
		obj.AddMember("aufPubkey", toJson(value.getAufPubkey(), alloc), alloc);
	})

	DEFINE_TO_JSON(GradidoCreation, {
		obj.AddMember("recipient", toJson(value.getRecipient(), alloc), alloc);
		obj.AddMember("targetDate", toJson(value.getTargetDate().getAsTimepoint(), alloc), alloc);
	})

	DEFINE_TO_JSON(GradidoTransfer, {
		obj.AddMember("sender", toJson(value.getSender(), alloc), alloc);
		obj.AddMember("recipient", toJson(value.getRecipient(), alloc), alloc);
	})

	DEFINE_TO_JSON(GradidoDeferredTransfer, {
		obj.AddMember("transfer", toJson(value.getTransfer(), alloc), alloc);
		obj.AddMember("timeout", toJson(value.getTimeoutDuration().getAsDuration(), alloc), alloc);
	})

	DEFINE_TO_JSON(GradidoRedeemDeferredTransfer, {
		obj.AddMember("transfer", toJson(value.getTransfer(), alloc), alloc);
		obj.AddMember("deferredTransferTransactionNr", value.getDeferredTransferTransactionNr(), alloc);
	})

	DEFINE_TO_JSON(GradidoTimeoutDeferredTransfer, {
		obj.AddMember("deferredTransferTransactionNr", value.getDeferredTransferTransactionNr(), alloc);
	})

	DEFINE_TO_JSON(RegisterAddress, {
		auto userPublicKey = value.getUserPublicKey();
		if (userPublicKey) {
			obj.AddMember("userPubkey", toJson(userPublicKey, alloc), alloc);
		}
		obj.AddMember("addressType", toJson(value.getAddressType(), alloc), alloc);
		auto nameHash = value.getNameHash();
		if (nameHash) {
			obj.AddMember("nameHash", toJson(nameHash, alloc), alloc);
		}
		auto accountPublicKey = value.getAccountPublicKey();
		if (accountPublicKey) {
			obj.AddMember("accountPubkey", toJson(accountPublicKey, alloc), alloc);
		}
		obj.AddMember("derivationIndex", value.getDerivationIndex(), alloc);
	})

	// superordinate transaction types

	DEFINE_TO_JSON(TransactionBody, {
		obj.AddMember("memos", toJson(value.getMemos(), alloc), alloc);
		obj.AddMember("createdAt", toJson(value.getCreatedAt().getAsTimepoint(), alloc), alloc);
		obj.AddMember("versionNumber", toJson(value.getVersionNumber(), alloc), alloc);
		obj.AddMember("type", toJson(value.getType(), alloc), alloc);
		auto otherCommunityIdIndex = value.getOtherCommunityIdIndex();
		if (otherCommunityIdIndex.has_value()) {
			auto otherCommunityIdString = g_appContext->getCommunityIds().getDataForIndex(otherCommunityIdIndex.value());
			if (!otherCommunityIdString.has_value()) {
				throw DictionaryMissingEntryException("couldn't find communityId", to_string(otherCommunityIdIndex.value()));
			}
			obj.AddMember("otherCommunity", toJson(otherCommunityIdString.value(), alloc), alloc);
		}
		switch (value.getTransactionType()) {
		case TransactionType::TRANSFER:
			obj.AddMember("transfer", toJson(*value.getTransfer(), alloc), alloc);
			break;
		case TransactionType::CREATION:
			obj.AddMember("creation", toJson(*value.getCreation(), alloc), alloc);
			break;
		case TransactionType::COMMUNITY_FRIENDS_UPDATE:
			obj.AddMember("communityFriendsUpdate", toJson(*value.getCommunityFriendsUpdate(), alloc), alloc);
			break;
		case TransactionType::REGISTER_ADDRESS:
			if (value.getRegisterAddress()) {
				obj.AddMember("registerAddress", toJson(value.getRegisterAddress().value(), alloc), alloc);
			}
			break;
		case TransactionType::DEFERRED_TRANSFER:
			obj.AddMember("deferredTransfer", toJson(*value.getDeferredTransfer(), alloc), alloc);
			break;
		case TransactionType::COMMUNITY_ROOT:
			if (value.getCommunityRoot()) {
				obj.AddMember("communityRoot", toJson(value.getCommunityRoot().value(), alloc), alloc);
			}
			break;
		case TransactionType::REDEEM_DEFERRED_TRANSFER:
			obj.AddMember("redeemDeferredTransfer", toJson(*value.getRedeemDeferredTransfer(), alloc), alloc);
			break;
		case TransactionType::TIMEOUT_DEFERRED_TRANSFER:
			obj.AddMember("timeoutDeferredTransfer", toJson(*value.getTimeoutDeferredTransfer(), alloc), alloc);
			break;
		case TransactionType::NONE: break;
		default: throw GradidoUnhandledEnum("missing toJson call", "TransactionType on transactionBody", magic_enum::enum_name(value.getTransactionType()).data());
		}
	})

	DEFINE_TO_JSON(LedgerAnchor, {
		auto type = value.getType();
		obj.AddMember("type", toJson(type, alloc), alloc);
		switch (type) {
		case LedgerAnchor::Type::IOTA_MESSAGE_ID:
			obj.AddMember("value", toJson(value.getIotaMessageId(), alloc), alloc);
			break;
		case LedgerAnchor::Type::HIERO_TRANSACTION_ID:
			obj.AddMember("value", toJson(value.getHieroTransactionId(), alloc), alloc);
			break;
		case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_ID:
		case LedgerAnchor::Type::LEGACY_GRADIDO_DB_COMMUNITY_ID:
		case LedgerAnchor::Type::LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
		case LedgerAnchor::Type::LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
		case LedgerAnchor::Type::LEGACY_GRADIDO_DB_USER_ID:
			obj.AddMember("value", value.getLegacyTransactionId(), alloc);
			break;
		case LedgerAnchor::Type::NODE_TRIGGER_TRANSACTION_ID:
			obj.AddMember("value", value.getNodeTriggeredTransactionId(), alloc);
			break;
		default:
			obj.AddMember("value", {}, alloc);
			break;
		}
	})

	DEFINE_TO_JSON(GradidoTransaction, {
		obj.AddMember("signatureMap", toJson(value.getSignatureMap().getSignaturePairs(), alloc), alloc);
		try {
			auto body = value.getTransactionBody();
			if (body) {
				obj.AddMember("bodyBytes", toJson(*body, alloc), alloc);
			}
		}
		catch (std::exception& ex) {
			obj.AddMember("bodyBytes", toJson(std::string(ex.what()), alloc), alloc);
		}
		obj.AddMember("pairingLedgerAnchor", toJson(value.getPairingLedgerAnchor(), alloc), alloc);
	})
	
	DEFINE_TO_JSON(ConfirmedTransaction, {
		obj.AddMember("id", value.getId(), alloc);
		auto gradidoTransaction = value.getGradidoTransaction();
		if (gradidoTransaction) {
			obj.AddMember("gradidoTransaction", toJson(*gradidoTransaction, alloc), alloc);
		}
		obj.AddMember("confirmedAt", toJson(value.getConfirmedAt().getAsTimepoint(), alloc), alloc);
		obj.AddMember("versionNumber", toJson(value.getVersionNumber(), alloc), alloc);
		auto runningHash = value.getRunningHash();
		if (runningHash) {
			obj.AddMember("runningHash", toJson(runningHash, alloc), alloc);
		}
		obj.AddMember("ledgerAnchor", toJson(value.getLedgerAnchor(), alloc), alloc);
		obj.AddMember("accountBalances", toJson(value.getAccountBalances(), alloc), alloc);
		obj.AddMember("balanceDerivationType", toJson(value.getBalanceDerivationType(), alloc), alloc);
	})
}
