#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/data/compact/CommunityRootTx.h"
#include "gradido_blockchain/interaction/validate/CommunityRootRole.h"
#include "gradido_blockchain/interaction/validate/Exceptions.h"
#include "gradido_blockchain/memory/Block.h"

#include "date/date.h"

#include <memory>

using memory::ConstBlockPtr;
using std::shared_ptr, std::make_shared;


namespace gradido {
	using data::ConfirmedTransaction;
	using data::compact::CommunityRootTx;
	namespace interaction {
		namespace validate {

			CommunityRootRole::CommunityRootRole(CommunityRootTx&& communityRoot)
				: mCommunityRoot(std::move(communityRoot)) 
			{
				// prepare for signature check
				mMinSignatureCount = 1;
				mRequiredSignPublicKeyIndices[0] = communityRoot.publicKeyIndex;
				mRequiredSignPublicKeyIndicesCount = 1;
			}

			void CommunityRootRole::run(Type type, ContextData& c) 
			{
				if ((type & Type::SINGLE) == Type::SINGLE) {
					if (
						!g_appContext->hasPublicKey(mCommunityRoot.publicKeyIndex) ||
						!g_appContext->hasPublicKey(mCommunityRoot.gmwPublicKeyIndex) ||
						!g_appContext->hasPublicKey(mCommunityRoot.aufPublicKeyIndex)
						) {
						throw TransactionValidationException("at least one public key index is invalid");
					}
					
					if (mCommunityRoot.gmwPublicKeyIndex == mCommunityRoot.aufPublicKeyIndex) {
						throw TransactionValidationException("gmw and auf are the same"); 
					}
					if (mCommunityRoot.publicKeyIndex == mCommunityRoot.gmwPublicKeyIndex) { 
						throw TransactionValidationException("gmw and pubkey are the same"); 
					}
					if (mCommunityRoot.aufPublicKeyIndex == mCommunityRoot.publicKeyIndex) { 
						throw TransactionValidationException("aufPubkey and pubkey are the same");
					}
				}
				if ((type & Type::PREVIOUS) == Type::PREVIOUS) {
					if (c.senderPreviousConfirmedTransaction) {
						throw TransactionValidationException("community root must be the first transaction in the blockchain!");
					}
				}
			}
		}
	}
}