#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_TRANSACTION_VALIDATION_EXCEPTIONS
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_TRANSACTION_VALIDATION_EXCEPTIONS

#include "gradido_blockchain/GradidoBlockchainException.h"
#include <string>

#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/model/protobufWrapper/TransactionBody.h"

#include "Poco/DateTime.h"

namespace model {
	namespace gradido {

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationException : public GradidoBlockchainException
		{
		public:
			explicit TransactionValidationException(const char* what) noexcept;
			virtual ~TransactionValidationException() {}

			std::string getFullString() const noexcept;

			inline TransactionValidationException& setMemo(const std::string& memo) { mTransactionMemo = memo; return *this; }
			inline const std::string& getMemo() const { return mTransactionMemo; }

			inline TransactionValidationException& setTransactionType(TransactionType type) { mType = type; return *this; }
			inline TransactionType getTransactionType() { return mType; }

			TransactionValidationException& setTransactionBody(const TransactionBody* transactionBody);
			inline const char* getTransactionTypeString() { return TransactionBody::transactionTypeToString(mType); }
		protected:
			std::string mTransactionMemo;
			TransactionType mType;
		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationInvalidInputException : public TransactionValidationException
		{
		public:
			explicit TransactionValidationInvalidInputException(const char* what, const char* fieldname, const char* fieldType = nullptr) noexcept;
			std::string getFullString() const noexcept;

			inline const std::string& getFieldName() const { return mFieldName; }
			inline const std::string& getFieldType() const { return mFieldType; }
		protected:
			std::string mFieldName;
			std::string mFieldType;
		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationInvalidSignatureException : public TransactionValidationException
		{
		public:
			explicit TransactionValidationInvalidSignatureException(const char* what, MemoryBin* pubkey, MemoryBin* signature, const std::string& bodyBytes = "") noexcept;
			explicit TransactionValidationInvalidSignatureException(
				const char* what, const std::string& pubkey, const std::string& signature, const std::string& bodyBytes = "") noexcept;
			~TransactionValidationInvalidSignatureException();

			std::string getFullString() const noexcept;
			inline const MemoryBin* getPubkey() const { return mPubkey; }
			inline const MemoryBin* getSignature() const { return mSignature; }
			inline const std::string& getBodyBytes() const { return mBodyBytes; }

		protected:
			MemoryBin* mPubkey;
			MemoryBin* mSignature;
			std::string mBodyBytes;
		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationForbiddenSignException : public TransactionValidationException
		{
		public: 
			explicit TransactionValidationForbiddenSignException(MemoryBin* forbiddenPubkey);
			~TransactionValidationForbiddenSignException();

			std::string getFullString() const noexcept;
		protected:
			MemoryBin* mForbiddenPubkey;

		};

	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_TRANSACTION_VALIDATION_EXCEPTIONS

