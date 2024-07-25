#include "gradido_blockchain/interaction/toJson/TransactionBodyRole.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "magic_enum/magic_enum.hpp"

using namespace rapidjson;
using namespace magic_enum;

namespace gradido {
	namespace interaction {
		namespace toJson {
			Value TransactionBodyRole::composeJson(rapidjson::Document& rootDocument) const
			{
				Value d(kObjectType);
				auto& alloc = rootDocument.GetAllocator();
				d.AddMember("memo", Value(mBody.memo.data(), alloc), alloc);
				d.AddMember("createdAt", Value(DataTypeConverter::timePointToString(mBody.createdAt).data(), alloc), alloc);
				d.AddMember("versionNumber", Value(mBody.versionNumber.data(), alloc), alloc);
				d.AddMember("type", Value(enum_name(mBody.type).data(), alloc), alloc);
				if (!mBody.otherGroup.empty()) {
					d.AddMember("otherGroup", Value(mBody.otherGroup.data(), alloc), alloc);
				}
				if (mBody.isTransfer()) {
					d.AddMember("transfer", gradidoTransfer(*mBody.transfer.get(), d, rootDocument), alloc);
				} 
				else if (mBody.isCreation()) {
					auto creation = mBody.creation;
					Value v(kObjectType);
					v.AddMember("recipient", transferAmount(creation->recipient, d, rootDocument), alloc);
					v.AddMember("targetDate", Value(DataTypeConverter::timePointToString(creation->targetDate).data(), alloc), alloc);
					d.AddMember("creation", v, alloc);
				}
				else if (mBody.isCommunityFriendsUpdate()) {
					Value v(kObjectType);
					v.AddMember("colorFusion", mBody.communityFriendsUpdate->colorFusion, alloc);
					d.AddMember("communityFriendsUpdate", v, alloc);
				}
				else if (mBody.isRegisterAddress()) {
					auto registerAddress = mBody.registerAddress;
					Value v(kObjectType);
					if (registerAddress->userPubkey) {
						v.AddMember("userPubkey", Value(registerAddress->userPubkey->convertToHex().data(), alloc), alloc);
					}
					v.AddMember("addressType", Value(enum_name(registerAddress->addressType).data(), alloc), alloc);
					if (registerAddress->nameHash) {
						v.AddMember("nameHash", Value(registerAddress->nameHash->convertToHex().data(), alloc), alloc);
					}
					if (registerAddress->accountPubkey) {
						v.AddMember("accountPubkey", Value(registerAddress->accountPubkey->convertToHex().data(), alloc), alloc);
					}
					v.AddMember("derivationIndex", registerAddress->derivationIndex, alloc);
					d.AddMember("registerAddress", v, alloc);
				}
				else if (mBody.isDeferredTransfer()) {
					auto deferredTransfer = mBody.deferredTransfer;
					Value v(kObjectType);
					v.AddMember("transfer", gradidoTransfer(deferredTransfer->transfer, d, rootDocument), alloc);
					v.AddMember("timeout", Value(DataTypeConverter::timePointToString(deferredTransfer->timeout).data(), alloc), alloc);
					d.AddMember("deferredTransfer", v, alloc);
				}
				else if (mBody.isCommunityRoot()) {
					auto communityRoot = mBody.communityRoot;
					Value v(kObjectType);
					if (communityRoot->pubkey) {
						v.AddMember("pubkey", Value(communityRoot->pubkey->convertToHex().data(), alloc), alloc);
					}
					if (communityRoot->gmwPubkey) {
						v.AddMember("gmwPubkey", Value(communityRoot->gmwPubkey->convertToHex().data(), alloc), alloc);
					}
					if (communityRoot->aufPubkey) {
						v.AddMember("aufPubkey", Value(communityRoot->aufPubkey->convertToHex().data(), alloc), alloc);
					}
					d.AddMember("communityRoot", v, alloc);
				}
				return d;
			}

			Value TransactionBodyRole::gradidoTransfer(const data::GradidoTransfer& data, Value& d, Document& rootDocument) const
			{
				auto alloc = rootDocument.GetAllocator();
				Value v(kObjectType);
				v.AddMember("sender", transferAmount(data.sender, d, rootDocument), alloc);
				if (data.recipient) {
					v.AddMember("recipient", Value(data.recipient->convertToHex().data(), alloc), alloc);
				}
				return v;  
			}
			rapidjson::Value TransactionBodyRole::transferAmount(const data::TransferAmount& data, Value& d, Document& rootDocument) const
			{
				auto alloc = rootDocument.GetAllocator();
				Value v(kObjectType);
				if (data.pubkey) {
					v.AddMember("pubkey", Value(data.pubkey->convertToHex().data(), alloc), alloc);
				}
				v.AddMember("amount", Value(data.amount.toString().data(), alloc), alloc);
				if (!data.communityId.empty()) {
					v.AddMember("communityId", Value(data.communityId.data(), alloc), alloc);
				}
				return v;
			}
		}
	}
}