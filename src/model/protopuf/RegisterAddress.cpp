#include "gradido_blockchain/model/protopuf/RegisterAddress.h"

namespace model {
	namespace protopuf {
		RegisterAddress::RegisterAddress(const RegisterAddressMessage& data)
			: mUserPubkey(std::make_shared<memory::Block>(data["user_pubkey"_f].value())),
			mAddressType(data["address_type"_f].value()),
			mDerivationIndex(data["derivation_index"_f].value())
		{
			if (data["name_hash"_f].has_value()) {
				mNameHash = std::make_shared<memory::Block>(data["name_hash"_f].value());
			}
			if (data["account_pubkey"_f].has_value()) {
				mAccountPubkey = std::make_shared<memory::Block>(data["account_pubkey"_f].value());
			}
		}

		RegisterAddress::RegisterAddress(
			ConstMemoryBlockPtr userPubkey,
			AddressType addressType,
			uint32_t derivationIndex /* = 1*/,
			ConstMemoryBlockPtr nameHash /* = nullptr */,
			ConstMemoryBlockPtr accountPubkey /* = nullptr */
		)
			: mUserPubkey(userPubkey), mAddressType(addressType),
			mNameHash(nameHash), mAccountPubkey(accountPubkey), mDerivationIndex(derivationIndex)
		{

		}
	}
}