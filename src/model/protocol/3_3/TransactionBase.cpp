#include "gradido_blockchain/model/protobufWrapper/TransactionBase.h"
#include "gradido_blockchain/model/protobufWrapper/GradidoTransaction.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionValidationExceptions.h"
#include "gradido_blockchain/crypto/KeyPairEd25519.h"

#include "gradido_blockchain/lib/Decay.h"

#include <iomanip>
#include <sodium.h>
#include <sstream>
#include <regex>

namespace model {
	namespace gradido {

		std::regex g_RegExGroupAlias("^[a-z0-9-]{3,120}$");

		
		bool TransactionBase::isValidGroupAlias(const std::string& groupAlias)
		{
			return std::regex_match(groupAlias, g_RegExGroupAlias);
		}


		TransactionBase::TransactionBase()
			: mMinSignatureCount(0), mIsPrepared(false)
		{

		}

		TransactionBase::~TransactionBase()
		{
		}


		bool TransactionBase::checkRequiredSignatures(const proto::gradido::SignatureMap* sig_map) const
		{
			assert(mMinSignatureCount);
			
			// not enough
			if (mMinSignatureCount > sig_map->sig_pair_size()) {
				throw TransactionValidationMissingSignException(sig_map->sig_pair_size(), mMinSignatureCount);
			}
			// enough
			if (!mRequiredSignPublicKeys.size() && !mForbiddenSignPublicKeys.size()) {
				return true;
			}
			// check if specific signatures can be found
			
			// prepare, make a copy from the vector, because entries will be removed from it
			std::vector<memory::Block> required_keys = mRequiredSignPublicKeys;			
			
			for (auto it = sig_map->sig_pair().begin(); it != sig_map->sig_pair().end(); it++) 
			{
				auto pubkey_size = it->pubkey().size();
				assert(pubkey_size == ED25519_PUBLIC_KEY_SIZE);
				
				// check for forbidden key
				if (isPublicKeyForbidden((const unsigned char*)it->pubkey().data())) {
					throw TransactionValidationForbiddenSignException(it->pubkey());
				}

				// compare with required keys
				for (auto it3 = required_keys.begin(); it3 != required_keys.end(); it3++) 
				{
					assert((*it3).size() == ED25519_PUBLIC_KEY_SIZE);
					if (0 == memcmp((*it3).data(), it->pubkey().data(), pubkey_size)) 
					{
						it3 = required_keys.erase(it3);
						break;
					}
				}
			}
			
			if (!required_keys.size()) return true;

			// TODO: check that given pubkeys are registered for same group 

			throw TransactionValidationRequiredSignMissingException(required_keys);
			
		}

		bool TransactionBase::isPublicKeyRequired(const unsigned char* pubkey) const
		{
			std::lock_guard<std::recursive_mutex> _lock(mWorkMutex);

			for (auto it = mRequiredSignPublicKeys.begin(); it != mRequiredSignPublicKeys.end(); it++) {
				if (memcmp((*it), pubkey, ED25519_PUBLIC_KEY_SIZE) == 0) {
					return true;
				}
			}
			return false;
		}
		
		bool TransactionBase::isPublicKeyForbidden(const unsigned char* pubkey) const
		{
			std::lock_guard<std::recursive_mutex> _lock(mWorkMutex);
			for (auto it = mForbiddenSignPublicKeys.begin(); it != mForbiddenSignPublicKeys.end(); it++) {
				if (memcmp((*it), pubkey, ED25519_PUBLIC_KEY_SIZE) == 0) {
					return true;
				}
			}
			return false;
		}

		/*
			TRANSACTION_NONE,
			TRANSACTION_CREATION,
			TRANSACTION_TRANSFER,
			TRANSACTION_GROUP_FRIENDS_UPDATE,
			TRANSACTION_REGISTER_ADDRESS,
			TRANSACTION_GLOBAL_GROUP_ADD,
			TRANSACTION_DEFERRED_TRANSFER
		*/
		const char* TransactionBase::getTransactionTypeString(TransactionType type)
		{
			switch (type) {
			case TRANSACTION_NONE: return "none";
			case TRANSACTION_CREATION: return "creation";
			case TRANSACTION_TRANSFER: return "transfer";
			case TRANSACTION_GROUP_FRIENDS_UPDATE: return "group friends update";
			case TRANSACTION_REGISTER_ADDRESS: return "register address";
			case TRANSACTION_DEFERRED_TRANSFER: return "deferred transfer";
			case TRANSACTION_COMMUNITY_ROOT: return "community root";
			default: return "<unknown>";
			}
		}
#ifdef USE_MPFR
		void TransactionBase::amountToString(std::string* strPointer, mpfr_ptr amount)
		{
			mpfr_exp_t exp_temp;
			auto str = mpfr_get_str(nullptr, &exp_temp, 10, 40, amount, gDefaultRound);
			if (amount->_mpfr_sign == -1 && exp_temp > 0) {
				exp_temp++;
			}
			auto strLength = strlen(str);
			strPointer->clear();
			// example: str = "100000000000000000000000000000000000000000000"
			//     exp_temp = 4
			// target       = 1000
			// 0x0000020e41491570 "-000000000000000000000000000000000000000000000"

			int i = strLength - 1;
			for (; i >= 0; i--) {
				if (str[i] != '0') break;
			}
			if (i == -1) {
				strPointer->assign("0");
			}
			else if (exp_temp > 0) {
				if (exp_temp >= i) {
					strPointer->reserve(exp_temp);
					strPointer->append(str, exp_temp);
				}
				else {
					strPointer->reserve(i + 2);
					strPointer->append(str, exp_temp);
					strPointer->push_back('.');
					strPointer->append(&str[exp_temp], i - exp_temp);
				}	
			}
			else if (!exp_temp) {
				if (i == 0) {
					strPointer->assign("0");
				}
				else if (amount->_mpfr_sign == -1) {
					strPointer->reserve(i + 4);
					strPointer->assign("-0.");
					strPointer->append(&str[1], i-1);
				}
				else {
					strPointer->reserve(i + 3);
					strPointer->assign("0.");
					strPointer->append(str, i);
				}
			}
			else if (exp_temp < 0) {
				strPointer->reserve(i + 3 - exp_temp);
				if (amount->_mpfr_sign == -1) {
					strPointer->assign("-0."); 
				}
				else {
					strPointer->assign("0.");
				}
				strPointer->append(-exp_temp, '0');
				if (amount->_mpfr_sign == -1) {
					strPointer->append(&str[1], i-1);
				}
				else {
					strPointer->append(str, i);
				}
			}
			mpfr_free_str(str);
		}
#endif // USE_MPFR

		void TransactionBase::validate25519PublicKey(const std::string& ed25519PublicKey, const char* name)
		{
			if (ed25519PublicKey.size() != ED25519_PUBLIC_KEY_SIZE) {
				throw TransactionValidationInvalidInputException("invalid size", name, "public key");
			}
			if (ed25519PublicKey.find_first_not_of('\0') == std::string::npos) {
				throw TransactionValidationInvalidInputException("empty", name, "public key");
			}
		}
	}
}

