#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/data/compact/RegisterAddressTx.h"
#include "gradido_blockchain/serialization/toJson.h"

using namespace rapidjson;
using gradido::g_appContext;
using gradido::data::compact::CommunityRootTx, gradido::data::compact::RegisterAddressTx;
using serialization::toJson;

namespace serialization { 
	DEFINE_TO_JSON(CommunityRootTx, {	
		obj.AddMember("pubkey", value.publicKeyIndex, alloc);
		obj.AddMember("gmwPubkey", value.gmwPublicKeyIndex, alloc);
		obj.AddMember("aufPubkey", value.aufPublicKeyIndex, alloc);
	})

	DEFINE_TO_JSON(RegisterAddressTx, {
		obj.AddMember("userPubkey", value.userPublicKeyIndex, alloc);
		obj.AddMember("addressType", toJson(value.addressType, alloc), alloc);
		auto nameHash = g_appContext->getUserNameHashs().getDataForIndex(value.nameHashIndex);
		if (nameHash) {
			obj.AddMember("nameHash", toJson(nameHash->convertToHex(), alloc), alloc);
		}
		obj.AddMember("accountPubkey", value.accountPublicKeyIndex, alloc);
		obj.AddMember("derivationIndex", value.derivationIndex, alloc);
	})

}