#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/serialization/toJson.h"

using namespace rapidjson;
using gradido::g_appContext;
using gradido::data::compact::CommunityRootTx;
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