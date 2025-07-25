#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/toJson/TransactionBodyRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	using namespace data;
	namespace interaction {
		namespace toJson {
			Value TransactionBodyRole::composeJson(rapidjson::Document& rootDocument) const
			{
				Value d(kObjectType);
				auto& alloc = rootDocument.GetAllocator();
				Value memos(kArrayType);
				for (auto& encryptedMemo : mBody.getMemos()) {
					Value memo(kObjectType);
					memo.AddMember("type", Value(enum_name(encryptedMemo.getKeyType()).data(), alloc), alloc);
					if (MemoKeyType::PLAIN == encryptedMemo.getKeyType()) {
						memo.AddMember("memo", Value(encryptedMemo.getMemo().copyAsString().data(), alloc), alloc);
					} else {
						memo.AddMember("memo", Value(encryptedMemo.getMemo().convertToBase64().data(), alloc), alloc);
					}
					memos.PushBack(memo, alloc);
				}
				d.AddMember("memos", memos, alloc);
				// d.AddMember("memo", Value(mBody.getMemo().data(), alloc), alloc);
				d.AddMember("createdAt", Value(DataTypeConverter::timePointToString(mBody.getCreatedAt()).data(), alloc), alloc);
				d.AddMember("versionNumber", Value(mBody.getVersionNumber().data(), alloc), alloc);
				d.AddMember("type", Value(enum_name(mBody.getType()).data(), alloc), alloc);
				if (!mBody.getOtherGroup().empty()) {
					d.AddMember("otherGroup", Value(mBody.getOtherGroup().data(), alloc), alloc);
				}
				if (mBody.isTransfer()) {
					d.AddMember("transfer", gradidoTransfer(*mBody.getTransfer().get(), d, rootDocument), alloc);
				} 
				else if (mBody.isCreation()) {
					auto creation = mBody.getCreation();
					Value v(kObjectType);
					v.AddMember("recipient", transferAmount(creation->getRecipient(), d, rootDocument), alloc);
					v.AddMember("targetDate", Value(DataTypeConverter::timePointToString(creation->getTargetDate()).data(), alloc), alloc);
					d.AddMember("creation", v, alloc);
				}
				else if (mBody.isCommunityFriendsUpdate()) {
					Value v(kObjectType);
					v.AddMember("colorFusion", mBody.getCommunityFriendsUpdate()->getColorFusion(), alloc);
					d.AddMember("communityFriendsUpdate", v, alloc);
				}
				else if (mBody.isRegisterAddress()) {
					auto registerAddress = mBody.getRegisterAddress();
					Value v(kObjectType);
					if (registerAddress->getUserPublicKey()) {
						v.AddMember("userPubkey", Value(registerAddress->getUserPublicKey()->convertToHex().data(), alloc), alloc);
					}
					v.AddMember("addressType", Value(enum_name(registerAddress->getAddressType()).data(), alloc), alloc);
					if (registerAddress->getNameHash()) {
						v.AddMember("nameHash", Value(registerAddress->getNameHash()->convertToHex().data(), alloc), alloc);
					}
					if (registerAddress->getAccountPublicKey()) {
						v.AddMember("accountPubkey", Value(registerAddress->getAccountPublicKey()->convertToHex().data(), alloc), alloc);
					}
					v.AddMember("derivationIndex", registerAddress->getDerivationIndex(), alloc);
					d.AddMember("registerAddress", v, alloc);
				}
				else if (mBody.isDeferredTransfer()) {
					auto deferredTransfer = mBody.getDeferredTransfer();
					Value v(kObjectType);
					v.AddMember("transfer", gradidoTransfer(deferredTransfer->getTransfer(), d, rootDocument), alloc);
					v.AddMember("timeout", Value(DataTypeConverter::timespanToString(deferredTransfer->getTimeoutDuration()).data(), alloc), alloc);
					d.AddMember("deferredTransfer", v, alloc);
				}
				else if (mBody.isRedeemDeferredTransfer()) {
					auto redeemDeferredTransfer = mBody.getRedeemDeferredTransfer();
					Value v(kObjectType);
					v.AddMember("transfer", gradidoTransfer(redeemDeferredTransfer->getTransfer(), d, rootDocument), alloc);
					v.AddMember("deferredTransferTransactionNr", redeemDeferredTransfer->getDeferredTransferTransactionNr(), alloc);
					d.AddMember("redeemDeferredTransfer", v, alloc);
				}
				else if (mBody.isTimeoutDeferredTransfer()) {
					auto timeoutDeferredTransfer = mBody.getTimeoutDeferredTransfer();
					Value v(kObjectType);
					v.AddMember("deferredTransferTransactionNr", timeoutDeferredTransfer->getDeferredTransferTransactionNr(), alloc);
					d.AddMember("timeoutDeferredTransfer", v, alloc);
				}
				else if (mBody.isCommunityRoot()) {
					auto communityRoot = mBody.getCommunityRoot();
					Value v(kObjectType);
					if (communityRoot->getPublicKey()) {
						v.AddMember("pubkey", Value(communityRoot->getPublicKey()->convertToHex().data(), alloc), alloc);
					}
					if (communityRoot->getGmwPubkey()) {
						v.AddMember("gmwPubkey", Value(communityRoot->getGmwPubkey()->convertToHex().data(), alloc), alloc);
					}
					if (communityRoot->getAufPubkey()) {
						v.AddMember("aufPubkey", Value(communityRoot->getAufPubkey()->convertToHex().data(), alloc), alloc);
					}
					d.AddMember("communityRoot", v, alloc);
				}
				return d;
			}

			Value TransactionBodyRole::gradidoTransfer(const data::GradidoTransfer& data, Value& d, Document& rootDocument) const
			{
				auto alloc = rootDocument.GetAllocator();
				Value v(kObjectType);
				v.AddMember("sender", transferAmount(data.getSender(), d, rootDocument), alloc);
				if (data.getRecipient()) {
					v.AddMember("recipient", Value(data.getRecipient()->convertToHex().data(), alloc), alloc);
				}
				return v;  
			}
			rapidjson::Value TransactionBodyRole::transferAmount(const data::TransferAmount& data, Value& d, Document& rootDocument) const
			{
				auto alloc = rootDocument.GetAllocator();
				Value v(kObjectType);
				if (data.getPublicKey()) {
					v.AddMember("pubkey", Value(data.getPublicKey()->convertToHex().data(), alloc), alloc);
				}
				v.AddMember("amount", Value(data.getAmount().toString().data(), alloc), alloc);
				if (!data.getCommunityId().empty()) {
					v.AddMember("communityId", Value(data.getCommunityId().data(), alloc), alloc);
				}
				return v;
			}
		}
	}
}