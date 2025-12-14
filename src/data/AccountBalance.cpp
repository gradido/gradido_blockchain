#include "gradido_blockchain/data/AccountBalance.h"
#include "gradido_blockchain/memory/Block.h"

using memory::Block, memory::ConstBlockPtr;
using std::string;

namespace gradido {
	namespace data {
		AccountBalance::AccountBalance() 
		{
		}
		
		AccountBalance::AccountBalance(ConstBlockPtr publicKey, GradidoUnit balance, const string& communityId)
			: mPublicKey(publicKey), mBalance(balance), mCommunityId(communityId) 
		{
		}

		AccountBalance::~AccountBalance() 
		{
		}
	}
}