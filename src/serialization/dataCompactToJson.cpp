#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"
#include "gradido_blockchain/serialization/toJson.h"

using namespace rapidjson;
using gradido::g_appContext;
using gradido::data::compact::CommunityRootTx, gradido::data::compact::RegisterAddressTx;
using serialization::toJson;

DEFINE_TO_JSON(CommunityRootTx, {
	auto publicKey = g_appContext->getPublicKey(value.publicKeyIndex);
	if (publicKey) {
		obj.AddMember("pubkey", toJson(publicKey.value().convertToHex(), alloc), alloc);
	}
	
	auto gmwPubkkey = g_appContext->getPublicKey(value.gmwPublicKeyIndex);
	if (gmwPubkkey) {
		obj.AddMember("gmwPubkey", toJson(gmwPubkkey.value().convertToHex(), alloc), alloc);
	}
		
	auto aufPubkey = g_appContext->getPublicKey(value.aufPublicKeyIndex);
	if (aufPubkey) {
		obj.AddMember("aufPubkey", toJson(aufPubkey.value().convertToHex(), alloc), alloc);
	}
})

DEFINE_TO_JSON(RegisterAddressTx, {
		auto userPublicKey = g_appContext->getPublicKey(value.userPublicKeyIndex);
		if (userPublicKey) {
			obj.AddMember("userPubkey", toJson(userPublicKey.value().convertToHex(), alloc), alloc);
		}
		obj.AddMember("addressType", toJson(value.addressType, alloc), alloc);
		auto nameHash = g_appContext->getUseNameHashs().getDataForIndex(value.nameHashIndex);
		if (nameHash) {
			obj.AddMember("nameHash", toJson(nameHash.value().convertToHex(), alloc), alloc);
		}
		auto accountPublicKey = g_appContext->getPublicKey(value.accountPublicKeyIndex);
		if (accountPublicKey) {
			obj.AddMember("accountPubkey", toJson(accountPublicKey.value().convertToHex(), alloc), alloc);
		}
		obj.AddMember("derivationIndex", value.derivationIndex, alloc);
	})