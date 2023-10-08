#include "gradido_blockchain/model/protobufWrapper/CommunityRoot.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/model/IGradidoBlockchain.h"

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
			auto mm = MemoryManager::getInstance();
			auto pubkey_copy = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
			assert(pubkey_copy);
			memcpy(*pubkey_copy, mProtoCommunityRoot.pubkey().data(), crypto_sign_PUBLICKEYBYTES);
			mRequiredSignPublicKeys.push_back(pubkey_copy);

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
			auto mm = MemoryManager::getInstance();

			if ((level & TRANSACTION_VALIDATION_SINGLE) == TRANSACTION_VALIDATION_SINGLE) {
				const auto& pubkey = mProtoCommunityRoot.pubkey();
				const auto& aufPubkey = mProtoCommunityRoot.auf_pubkey();
				const auto& gmwPubkey = mProtoCommunityRoot.gmw_pubkey();

				if (pubkey.size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "pubkey", "public key");
				}
				if (aufPubkey.size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "aufPubkey", "public key");
				}
				if (gmwPubkey.size() != crypto_sign_PUBLICKEYBYTES) {
					throw TransactionValidationInvalidInputException("invalid size", "gmwPubkey", "public key");
				}
				if (0 == memcmp(gmwPubkey.data(), aufPubkey.data(), crypto_sign_PUBLICKEYBYTES)) {
					throw TransactionValidationException("gmw and auf are the same");
				}
				if (0 == memcmp(pubkey.data(), gmwPubkey.data(), crypto_sign_PUBLICKEYBYTES)) {
					throw TransactionValidationException("gmw and pubkey are the same");
				}
				if (0 == memcmp(aufPubkey.data(), pubkey.data(), crypto_sign_PUBLICKEYBYTES)) {
					throw TransactionValidationException("aufPubkey and pubkey are the same");
				}
				auto empty = mm->getMemory(crypto_sign_PUBLICKEYBYTES);
				memset(*empty, 0, crypto_sign_PUBLICKEYBYTES);
				if (0 == memcmp(pubkey.data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					mm->releaseMemory(empty);
					throw TransactionValidationInvalidInputException("empty", "pubkey", "public key");
				}
				if (0 == memcmp(gmwPubkey.data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					mm->releaseMemory(empty);
					throw TransactionValidationInvalidInputException("empty", "gmwPubkey", "public key");
				}
				if (0 == memcmp(aufPubkey.data(), *empty, crypto_sign_PUBLICKEYBYTES)) {
					mm->releaseMemory(empty);
					throw TransactionValidationInvalidInputException("empty", "aufPubkey", "public key");
				}
				mm->releaseMemory(empty);
			}
			if ((level & TRANSACTION_VALIDATION_SINGLE_PREVIOUS) == TRANSACTION_VALIDATION_SINGLE_PREVIOUS) {
				assert(blockchain);
				if (!blockchain->getLastTransaction().isNull()) {
					throw TransactionValidationException("community root must be the first transaction in the blockchain!");
				}
			}
			return true;
		}

		std::vector<MemoryBin*> CommunityRoot::getInvolvedAddresses() const
		{
			auto mm = MemoryManager::getInstance();
			std::vector<MemoryBin*> result;

			auto userPubkey = getPubkey();
			if (userPubkey) {
				result.push_back(userPubkey);
			}
			auto gmwPubkey = getGMWPubkey();
			if (gmwPubkey) {
				result.push_back(gmwPubkey);
			}
			auto aufPubkey = getAUFPubkey();
			if (aufPubkey) {
				result.push_back(aufPubkey);
			}
			return result;
		}
		bool CommunityRoot::isInvolved(const std::string pubkeyString) const
		{
			return
				mProtoCommunityRoot.pubkey() == pubkeyString ||
				mProtoCommunityRoot.gmw_pubkey() == pubkeyString ||
				mProtoCommunityRoot.auf_pubkey() == pubkeyString
				;
		}

		const std::string& CommunityRoot::getPubkeyString() const
		{
			return mProtoCommunityRoot.pubkey();
		}

		const std::string& CommunityRoot::getGMWPubkeyString() const
		{
			return mProtoCommunityRoot.gmw_pubkey();
		}

		const std::string& CommunityRoot::getAUFPubkeyString() const
		{
			return mProtoCommunityRoot.auf_pubkey();
		}
	}
}