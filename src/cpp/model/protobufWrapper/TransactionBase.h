/*!
*
* \author: einhornimmond
*
* \date: 25.10.19
*
* \brief: Interface for Transaction Objects
*/
#ifndef GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_BASE_INCLUDE
#define GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_BASE_INCLUDE

#pragma warning(disable:4800)

#include "gradido/BasicTypes.pb.h"
#include "../../SingletonManager/MemoryManager.h"
#include "lib/MultithreadContainer.h"

namespace model {
	namespace gradido {

		class TransactionBase : public UniLib::lib::MultithreadContainer
		{
		public:
			TransactionBase(const std::string& memo);
			virtual ~TransactionBase();
			//! \return 0 if ok, < 0 if error, > 0 if not implemented
			virtual int prepare() {return 1;}
			virtual bool validate() = 0;

			static std::string amountToString(google::protobuf::int64 amount);
			inline const std::string& getMemo() const { return mMemo; }

			//! \return true if all required signatures are found in signature pairs
			bool checkRequiredSignatures(const proto::gradido::SignatureMap* sig_map);
			//! \param pubkey pointer must point to valid unsigned char[KeyPairEd25519::getPublicKeySize()] array
			bool isPublicKeyRequired(const unsigned char* pubkey);
			//! \param pubkey pointer must point to valid unsigned char[KeyPairEd25519::getPublicKeySize()] array
			bool isPublicKeyForbidden(const unsigned char* pubkey);

			inline uint32_t getMinSignatureCount() { return mMinSignatureCount; }
			void setMinSignatureCount(uint32_t minSignatureCount) { mMinSignatureCount = minSignatureCount; }

			bool isValidGroupAlias(const std::string& groupAlias);

		protected:
			std::string mMemo;
			uint32_t mMinSignatureCount;
			bool mIsPrepared;
			std::vector<MemoryBin*> mRequiredSignPublicKeys;
			std::vector<MemoryBin*> mForbiddenSignPublicKeys;
		};
	}
}



#endif //GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_BASE_INCLUDE
