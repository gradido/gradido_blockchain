#include "gradido_blockchain/model/protobufWrapper/CommunityRoot.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

namespace model {
	namespace gradido {

		CommunityRoot::CommunityRoot(const proto::gradido::CommunityRoot& protoCommunityRoot)
			: mProtoCommunityRoot(protoCommunityRoot)
		{

		}

		CommunityRoot::~CommunityRoot()
		{

		}

		int CommunityRoot::prepare()
		{
			mMinSignatureCount = 1;

			mRequiredSignPublicKeys.push_back(mProtoCommunityRoot.pubkey());

			mIsPrepared = true;
			return 0;
		}

		bool CommunityRoot::validate(
			TransactionValidationLevel level/* = TRANSACTION_VALIDATION_SINGLE*/,
			IGradidoBlockchain* blockchain/* = nullptr*/,
			const ConfirmedTransaction* parentConfirmedTransaction/* = nullptr*/
		) const
		{
			LOCK_RECURSIVE;

			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				const auto& pubkey = mProtoCommunityRoot.pubkey();
				const auto& aufPubkey = mProtoCommunityRoot.auf_pubkey();
				const auto& gmwPubkey = mProtoCommunityRoot.gmw_pubkey();

				validate25519PublicKey(pubkey, "pubkey");
				validate25519PublicKey(aufPubkey, "aufPubkey");
				validate25519PublicKey(gmwPubkey, "gmwPubkey");
				
				if (gmwPubkey == aufPubkey) { throw TransactionValidationException("gmw and auf are the same"); }
				if (pubkey == gmwPubkey) { throw TransactionValidationException("gmw and pubkey are the same"); }
				if (aufPubkey == pubkey) { throw TransactionValidationException("aufPubkey and pubkey are the same"); }
			}
			if ((level & TRANSACTION_VALIDATION_SINGLE_PREVIOUS) == TRANSACTION_VALIDATION_SINGLE_PREVIOUS) {
				assert(blockchain);
				if (blockchain->getLastTransaction()) {
					throw TransactionValidationException("community root must be the first transaction in the blockchain!");
				}
			}
			return true;
		}

		std::vector<std::string_view> CommunityRoot::getInvolvedAddresses() const
		{
			return {
				mProtoCommunityRoot.pubkey(),
				mProtoCommunityRoot.gmw_pubkey(),
				mProtoCommunityRoot.auf_pubkey()
			};
		}
		bool CommunityRoot::isInvolved(const std::string& pubkeyString) const
		{
			return
				mProtoCommunityRoot.pubkey() == pubkeyString ||
				mProtoCommunityRoot.gmw_pubkey() == pubkeyString ||
				mProtoCommunityRoot.auf_pubkey() == pubkeyString
				;
		}

		const std::string& CommunityRoot::getPubkey() const
		{
			return mProtoCommunityRoot.pubkey();
		}

		const std::string& CommunityRoot::getGMWPubkey() const
		{
			return mProtoCommunityRoot.gmw_pubkey();
		}

		const std::string& CommunityRoot::getAUFPubkey() const
		{
			return mProtoCommunityRoot.auf_pubkey();
		}
	}
}