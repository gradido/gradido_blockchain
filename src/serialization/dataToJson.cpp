#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
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
#include "gradido_blockchain_core/types/ledger_anchor.h"
#include "gradido_blockchain_core/types/memo_key.h"

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
		if (GRDT_MEMO_KEY_PLAIN == value.getKeyType()) {
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
		obj.AddMember("type", toJson(value.getType(), alloc), alloc);
		auto otherCommunityIdIndex = value.getOtherCommunityIdIndex();
		if (otherCommunityIdIndex.has_value()) {
			auto otherCommunityIdString = g_appContext->getCommunityIds().getDataForIndex(otherCommunityIdIndex.value());
			if (!otherCommunityIdString.has_value()) {
				throw DictionaryMissingEntryException("couldn't find communityId", to_string(otherCommunityIdIndex.value()));
			}
			obj.AddMember("otherCommunity", toJson(otherCommunityIdString.value(), alloc), alloc);
		}
		if (value.isRegisterAddress()) {
			auto registerAddress = value.getRegisterAddress().value();
			Value regAddVal(kObjectType);
			regAddVal.AddMember("userPubkey",
				toJson(g_appContext->getPublicKey({ .communityIdIndex = value.getCommunityIdIndex(), .publicKeyIndex = registerAddress.userPublicKeyIndex }), alloc),
				alloc
			);
			regAddVal.AddMember("addressType", toJson(registerAddress.addressType, alloc), alloc);
			auto nameHash = g_appContext->getUserNameHashs().getDataForIndex(registerAddress.nameHashIndex);
			if (nameHash) {
				regAddVal.AddMember("nameHash", toJson(nameHash->convertToHex(), alloc), alloc);
			}
			regAddVal.AddMember("accountPubkey",
				toJson(g_appContext->getPublicKey({ .communityIdIndex = value.getCommunityIdIndex(), .publicKeyIndex = registerAddress.accountPublicKeyIndex }), alloc),
				alloc
			);
			regAddVal.AddMember("derivationIndex", registerAddress.derivationIndex, alloc);
			obj.AddMember("registerAddress", regAddVal, alloc);
		}
		else if (value.isCommunityRoot()) {
			auto communityRoot = value.getCommunityRoot().value();
			Value comRootVal(kObjectType);
			comRootVal.AddMember(
				"pubkey",
				toJson(g_appContext->getPublicKey({ .communityIdIndex = value.getCommunityIdIndex(), .publicKeyIndex = communityRoot.publicKeyIndex }), alloc),
				alloc
			);
			comRootVal.AddMember(
				"gmwPubkey",
				toJson(g_appContext->getPublicKey({ .communityIdIndex = value.getCommunityIdIndex(), .publicKeyIndex = communityRoot.gmwPublicKeyIndex }), alloc),
				alloc
			);
			comRootVal.AddMember(
				"aufPubkey",
				toJson(g_appContext->getPublicKey({ .communityIdIndex = value.getCommunityIdIndex(), .publicKeyIndex = communityRoot.aufPublicKeyIndex }), alloc),
				alloc
			);
			obj.AddMember("communityRoot", comRootVal, alloc);
		}
		else {
			switch (value.getTransactionType()) {
			case GRDT_TRANSACTION_TRANSFER:
				obj.AddMember("transfer", toJson(*value.getTransfer(), alloc), alloc);
				break;
			case GRDT_TRANSACTION_CREATION:
				obj.AddMember("creation", toJson(*value.getCreation(), alloc), alloc);
				break;
			case GRDT_TRANSACTION_COMMUNITY_FRIENDS_UPDATE:
				obj.AddMember("communityFriendsUpdate", toJson(*value.getCommunityFriendsUpdate(), alloc), alloc);
				break;
			case GRDT_TRANSACTION_DEFERRED_TRANSFER:
				obj.AddMember("deferredTransfer", toJson(*value.getDeferredTransfer(), alloc), alloc);
				break;
			case GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER:
				obj.AddMember("redeemDeferredTransfer", toJson(*value.getRedeemDeferredTransfer(), alloc), alloc);
				break;
			case GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER:
				obj.AddMember("timeoutDeferredTransfer", toJson(*value.getTimeoutDeferredTransfer(), alloc), alloc);
				break;
			case GRDT_TRANSACTION_NONE: break;
			default: throw GradidoUnhandledEnum("missing toJson call", "grdt_transaction on transactionBody", magic_enum::enum_name(value.getTransactionType()).data());
			}
		}
	})

	DEFINE_TO_JSON(LedgerAnchor, {
		auto type = value.getType();
		obj.AddMember("type", toJson(type, alloc), alloc);
		switch (type) {
		case GRDT_LEDGER_ANCHOR_HIERO_TRANSACTION_ID:
			obj.AddMember("value", toJson(value.toString(), alloc), alloc);
			break;
		case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_ID:
		case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_COMMUNITY_ID:
		case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_USER_ID:
		case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_CONTRIBUTION_ID:
		case GRDT_LEDGER_ANCHOR_LEGACY_GRADIDO_DB_TRANSACTION_LINK_ID:
			obj.AddMember("value", value.getLegacyGradidoDbId(), alloc);
			break;
		case GRDT_LEDGER_ANCHOR_NODE_TRIGGER_TRANSACTION_ID:
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
		auto runningHash = value.getRunningHash();
		if (runningHash) {
			obj.AddMember("runningHash", toJson(runningHash, alloc), alloc);
		}
		obj.AddMember("ledgerAnchor", toJson(value.getLedgerAnchor(), alloc), alloc);
		obj.AddMember("accountBalances", toJson(value.getAccountBalances(), alloc), alloc);
		obj.AddMember("balanceDerivationType", toJson(value.getBalanceDerivationType(), alloc), alloc);
	})
}
