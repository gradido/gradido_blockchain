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
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/MultithreadContainer.h"
#include "gradido_blockchain/export.h"



namespace model {
	class IGradidoBlockchain;

	namespace gradido {

		class GradidoTransaction;
		class GradidoBlock;
		

		enum GRADIDOBLOCKCHAIN_EXPORT TransactionType: uint8_t {
			TRANSACTION_NONE,
			TRANSACTION_CREATION,
			TRANSACTION_TRANSFER,
			TRANSACTION_GROUP_FRIENDS_UPDATE,
			TRANSACTION_REGISTER_ADDRESS,
			TRANSACTION_GLOBAL_GROUP_ADD,
			TRANSACTION_DEFERRED_TRANSFER
		};

		enum GRADIDOBLOCKCHAIN_EXPORT TransactionValidationLevel {
			// check only the transaction
			TRANSACTION_VALIDATION_SINGLE = 1,
			// check also with previous transaction
			TRANSACTION_VALIDATION_SINGLE_PREVIOUS = 2,
			// check all transaction from within date range
			// by creation automatic the same month
			TRANSACTION_VALIDATION_DATE_RANGE = 4,
			// check paired transaction on another group by cross group transactions
			TRANSACTION_VALIDATION_PAIRED = 8,
			// check all transactions in the group which connected with this transaction address(es)
			TRANSACTION_VALIDATION_CONNECTED_GROUP = 16,
			// check all transactions which connected with this transaction
			TRANSACTION_VALIDATION_CONNECTED_BLOCKCHAIN = 32
		};
	
		class GRADIDOBLOCKCHAIN_EXPORT TransactionBase : public MultithreadContainer
		{
		public:
			TransactionBase();
			virtual ~TransactionBase();
			//! \return 0 if ok, < 0 if error, > 0 if not implemented
			virtual int prepare() {return 1;}
			virtual bool validate(
				TransactionValidationLevel level = TRANSACTION_VALIDATION_SINGLE,
				IGradidoBlockchain* blockchain = nullptr,
				const GradidoBlock* parentGradidoBlock = nullptr
				) const = 0;
			//! \return caller need to clean up memory bins
			virtual std::vector<MemoryBin*> getInvolvedAddresses() const = 0;
			virtual uint32_t getCoinColor() const = 0;
			virtual bool isBelongToUs(const TransactionBase* pairingTransaction) const = 0;
	
			//! \return true if all required signatures are found in signature pairs
			bool checkRequiredSignatures(const proto::gradido::SignatureMap* sig_map) const;
			//! \param pubkey pointer must point to valid unsigned char[KeyPairEd25519::getPublicKeySize()] array
			bool isPublicKeyRequired(const unsigned char* pubkey);
			//! \param pubkey pointer must point to valid unsigned char[KeyPairEd25519::getPublicKeySize()] array
			bool isPublicKeyForbidden(const unsigned char* pubkey);

			inline uint32_t getMinSignatureCount() const { return mMinSignatureCount; }
			void setMinSignatureCount(uint32_t minSignatureCount) { mMinSignatureCount = minSignatureCount; }

			static bool isValidGroupAlias(const std::string& groupAlias);
			static const char* getTransactionTypeString(TransactionType type);
			static void amountToString(std::string* strPointer, mpfr_ptr amount);
			

		protected:
			uint32_t mMinSignatureCount;
			bool mIsPrepared;
			std::vector<MemoryBin*> mRequiredSignPublicKeys;
			std::vector<MemoryBin*> mForbiddenSignPublicKeys;

		};
	}
}



#endif //GRADIDO_LOGIN_SERVER_MODEL_TRANSACTION_BASE_INCLUDE
