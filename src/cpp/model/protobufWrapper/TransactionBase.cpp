#include "TransactionBase.h"

#include "Poco/RegularExpression.h"
#include "TransactionValidationExceptions.h"

#include <iomanip>
#include <sodium.h>
#include <sstream>

namespace model {
	namespace gradido {

		Poco::RegularExpression g_RegExGroupAlias("^[a-z0-9-]{3,120}");

		
		bool TransactionBase::isValidGroupAlias(const std::string& groupAlias)
		{
			return g_RegExGroupAlias.match(groupAlias);
		}


		TransactionBase::TransactionBase(const std::string& memo)
			: mMemo(memo), mMinSignatureCount(0), mIsPrepared(false)
		{

		}

		TransactionBase::~TransactionBase()
		{
			auto mm = MemoryManager::getInstance();
			for (auto it = mRequiredSignPublicKeys.begin(); it != mRequiredSignPublicKeys.end(); it++)
			{
				mm->releaseMemory(*it);
			}
			mRequiredSignPublicKeys.clear();
			for (auto it = mForbiddenSignPublicKeys.begin(); it != mForbiddenSignPublicKeys.end(); it++)
			{
				mm->releaseMemory(*it);
			}
			mForbiddenSignPublicKeys.clear();
		}


		std::string TransactionBase::amountToString(google::protobuf::int64 amount)
		{
			std::stringstream ss;
			double dAmount = amount / 10000.0;
			ss << std::fixed << std::setprecision(2) << dAmount;
			std::string amountString = ss.str();
			if (amountString.find('.') != amountString.npos) {
				int pointPlace = amountString.find('.');
				if (amountString.substr(pointPlace + 1) == "00") {
					amountString = amountString.substr(0, pointPlace);
				}
			}
			return amountString;
			//return ss.str();
		}

		bool TransactionBase::checkRequiredSignatures(const proto::gradido::SignatureMap* sig_map)
		{
			assert(mMinSignatureCount);
			
			// not enough
			if (mMinSignatureCount > sig_map->sigpair_size()) {
				return false;
			}
			// enough
			if (!mRequiredSignPublicKeys.size() && !mForbiddenSignPublicKeys.size()) {
				return true;
			}
			// check if specific signatures can be found
			static const char function_name[] = "TransactionBase::checkRequiredSignatures";
			// prepare
			std::vector<MemoryBin*> required_keys = mRequiredSignPublicKeys;
			
			bool forbidden_key_found = false;
			for (auto it = sig_map->sigpair().begin(); it != sig_map->sigpair().end(); it++) 
			{
				auto pubkey_size = it->pubkey().size();
				assert(pubkey_size == crypto_sign_PUBLICKEYBYTES);
				
				// check for forbidden key
				if (!forbidden_key_found && mForbiddenSignPublicKeys.size()) 
				{
					for (auto it2 = mForbiddenSignPublicKeys.begin(); it2 != mForbiddenSignPublicKeys.end(); it2++) {
						assert((*it2)->size() == crypto_sign_PUBLICKEYBYTES);
						if (0 == memcmp((*it2)->data(), it->pubkey().data(), pubkey_size)) 
						{
							auto forbiddenKey = MemoryManager::getInstance()->getFreeMemory(pubkey_size);
							forbiddenKey->copyFrom(*it2);

							throw TransactionValidationForbiddenSignException(forbiddenKey, getMemo());
						}
					}
				}

				// compare with required keys
				for (auto it3 = required_keys.begin(); it3 != required_keys.end(); it3++) 
				{
					assert((*it3)->size() == crypto_sign_PUBLICKEYBYTES);
					if (0 == memcmp((*it3)->data(), it->pubkey().data(), pubkey_size)) 
					{
						it3 = required_keys.erase(it3);
						break;
					}
				}
			}
			
			if (!required_keys.size()) return true;

			// TODO: check that given pubkeys are registered for same group 

			return false;
			
		}

		bool TransactionBase::isPublicKeyRequired(const unsigned char* pubkey)
		{
			std::lock_guard<std::recursive_mutex> _lock(mWorkMutex);

			for (auto it = mRequiredSignPublicKeys.begin(); it != mRequiredSignPublicKeys.end(); it++) {
				if (memcmp((*it)->data(), pubkey, crypto_sign_PUBLICKEYBYTES) == 0) {
					return true;
				}
			}
			return false;
		}
		
		bool TransactionBase::isPublicKeyForbidden(const unsigned char* pubkey)
		{
			std::lock_guard<std::recursive_mutex> _lock(mWorkMutex);
			for (auto it = mForbiddenSignPublicKeys.begin(); it != mForbiddenSignPublicKeys.end(); it++) {
				if (memcmp((*it)->data(), pubkey, crypto_sign_PUBLICKEYBYTES) == 0) {
					return true;
				}
			}
			return false;
		}
	}
}

