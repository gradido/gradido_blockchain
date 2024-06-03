#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_TRANSACTION_VALIDATION_EXCEPTIONS
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_TRANSACTION_VALIDATION_EXCEPTIONS

#include "gradido_blockchain/GradidoBlockchainException.h"
#include <string>

#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/model/protobufWrapper/GradidoTransaction.h"

#include "proto/gradido/register_address.pb.h"

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
			inline const char* getTransactionTypeString() const { return TransactionBody::transactionTypeToString(mType); }
		protected:
			std::string mTransactionMemo;
			TransactionType mType;
		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationInvalidInputException : public TransactionValidationException
		{
		public:
			explicit TransactionValidationInvalidInputException(const char* what, const char* fieldname, const char* fieldType = nullptr) noexcept;
			std::string getFullString() const noexcept;
			//! mainly for return as details for json requests
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

			inline const std::string& getFieldName() const { return mFieldName; }
			inline const std::string& getFieldType() const { return mFieldType; }
		protected:
			std::string mFieldName;
			std::string mFieldType;
		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationInvalidSignatureException : public TransactionValidationException
		{
		public:
			explicit TransactionValidationInvalidSignatureException(
				const char* what, const std::string& pubkey, const std::string& signature, const std::string& bodyBytes = "") noexcept;
			~TransactionValidationInvalidSignatureException();

			std::string getFullString() const noexcept;
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

			inline const std::string& getPubkey() const { return mPubkey; }
			inline const std::string& getSignature() const { return mSignature; }
			inline const std::string& getBodyBytes() const { return mBodyBytes; }

		protected:
			std::string getPubkeyHex() const noexcept;
			std::string getSignatureHex() const noexcept;
			std::string getBodyBytesBase64() const noexcept;

			std::string mPubkey;
			std::string mSignature;
			std::string mBodyBytes;
		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationForbiddenSignException : public TransactionValidationException
		{
		public:
			explicit TransactionValidationForbiddenSignException(const std::string& forbiddenPubkey) noexcept;
			~TransactionValidationForbiddenSignException();

			std::string getFullString() const noexcept;
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;
		protected:
			std::string getForbiddenPubkeyHex() const noexcept;
			std::string mForbiddenPubkey;

		};

		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationMissingSignException : public TransactionValidationException
		{
		public:
			explicit TransactionValidationMissingSignException(int currentSignCount, int requiredSignCount) noexcept;

			std::string getFullString() const noexcept;
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;
		protected:
			int mCurrentSignCount;
			int mRequiredSignCount;
		};


		class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationRequiredSignMissingException : public TransactionValidationException
		{
		public:
			explicit TransactionValidationRequiredSignMissingException(const std::vector<memory::Block>& missingPublicKeys) noexcept;

		protected:
			std::vector<std::string> mMissingPublicKeysHex;
		};

		class GRADIDOBLOCKCHAIN_EXPORT PairingTransactionNotMatchException : public TransactionValidationException
		{
		public:
			explicit PairingTransactionNotMatchException(const char* what, const std::string* serializedTransaction, const std::string* serializedPairingTransaction) noexcept;

			std::string getFullString() const noexcept;

		protected:
			std::unique_ptr<model::gradido::GradidoTransaction> mTransaction;
			std::unique_ptr<model::gradido::GradidoTransaction> mPairingTransaction;
		};

		class GRADIDOBLOCKCHAIN_EXPORT AddressAlreadyExistException : public TransactionValidationException
		{
		public:
			explicit AddressAlreadyExistException(const char* what, const std::string& addressHex, proto::gradido::RegisterAddress_AddressType addressType) noexcept;

			std::string getFullString() const noexcept;
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

		protected:
			std::string mAddressHex;
			proto::gradido::RegisterAddress_AddressType mAddressType;
		};

		class GRADIDOBLOCKCHAIN_EXPORT InsufficientBalanceException : public TransactionValidationException
		{
		public:
			explicit InsufficientBalanceException(const char* what, mpfr_ptr needed, mpfr_ptr exist) noexcept;
			std::string getFullString() const noexcept;
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

		protected:
			std::string mNeeded;
			std::string mExist;
		};

		class GRADIDOBLOCKCHAIN_EXPORT InvalidCreationException : public TransactionValidationException
		{
		public:
			explicit InvalidCreationException(
				const char* what,
				int targetMonth, int targetYear,
				const std::string& newCreationAmount,
				const std::string& alreadyCreatedBalance
			) noexcept;
			std::string getFullString() const noexcept;
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

		protected:
			int mTargetMonth;
			int mTargetYear;
			std::string mNewCreationAmount;
			std::string mAlreadyCreatedBalance;
		};

		class GRADIDOBLOCKCHAIN_EXPORT WrongAddressTypeException : public TransactionValidationException
		{
		public:
			explicit WrongAddressTypeException(
				const char* what,
				proto::gradido::RegisterAddress_AddressType type,
				const std::string& pubkeyString
			) noexcept;

			std::string getFullString() const noexcept;
			rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

		protected:
			proto::gradido::RegisterAddress_AddressType mType;
			std::string mPubkey;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_WRAPPER_TRANSACTION_VALIDATION_EXCEPTIONS

