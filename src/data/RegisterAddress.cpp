#include "gradido_blockchain/data/RegisterAddress.h"

namespace gradido {
	namespace data {

		std::vector<memory::ConstBlockPtr> RegisterAddress::getInvolvedAddresses() const
		{
			std::vector<memory::ConstBlockPtr> result;
			result.reserve(2);
			if (mUserPubkey) {
				result.push_back(mUserPubkey);
			}
			if (mAccountPubkey) {
				result.push_back(mAccountPubkey);
			}
			return result;
		}

		bool RegisterAddress::isInvolved(memory::ConstBlockPtr publicKey) const
		{
			if (publicKey->isTheSame(mUserPubkey) || publicKey->isTheSame(mAccountPubkey)) {
				return true;
			}
			return false;
		}
	}
}