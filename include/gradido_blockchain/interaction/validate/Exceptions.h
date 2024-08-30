#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_EXCEPTIONS_H

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/data/TransactionType.h"

#include "gradido_blockchain/GradidoUnit.h"

#include "magic_enum/magic_enum.hpp"
#include "rapidjson/document.h"

namespace gradido {
	namespace interaction {
		namespace validate {
			class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationException : public GradidoBlockchainException
			{
			public:
				explicit TransactionValidationException(const char* what) noexcept;
				virtual ~TransactionValidationException() {}

				std::string getFullString() const noexcept;

				inline TransactionValidationException& setMemo(const std::string& memo) { mTransactionMemo = memo; return *this; }
				inline const std::string& getMemo() const { return mTransactionMemo; }

				inline TransactionValidationException& setTransactionType(data::TransactionType type) { mType = type; return *this; }
				inline data::TransactionType getTransactionType() { return mType; }

				TransactionValidationException& setTransactionBody(const data::TransactionBody& transactionBody);
				inline std::string_view getTransactionTypeString() const { return magic_enum::enum_name(mType); }
			protected:
				std::string mTransactionMemo;
				data::TransactionType mType;
			};

			class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationInvalidInputException : public TransactionValidationException
			{
			public:
				explicit TransactionValidationInvalidInputException(
					const char* what,
					const char* fieldName,
					const char* fieldType = nullptr,
					const char* expected = nullptr,
					const char* actual = nullptr
				) noexcept;
				std::string getFullString() const noexcept;
				//! mainly for return as details for json requests
				rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

				inline const std::string& getFieldName() const { return mFieldName; }
				inline const std::string& getFieldType() const { return mFieldType; }
			protected:
				std::string mFieldName;
				std::string mFieldType;
				std::string mExpected;
				std::string mActual;
			};

			class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationInvalidSignatureException : public TransactionValidationException
			{
			public:
				explicit TransactionValidationInvalidSignatureException(
					const char* what, memory::ConstBlockPtr pubkey, memory::ConstBlockPtr signature, memory::ConstBlockPtr bodyBytes = nullptr) noexcept;
				~TransactionValidationInvalidSignatureException();

				std::string getFullString() const noexcept;
				rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

				inline memory::ConstBlockPtr getPubkey() const { return mPubkey; }
				inline memory::ConstBlockPtr getSignature() const { return mSignature; }
				inline memory::ConstBlockPtr getBodyBytes() const { return mBodyBytes; }

			protected:

				memory::ConstBlockPtr mPubkey;
				memory::ConstBlockPtr mSignature;
				memory::ConstBlockPtr mBodyBytes;
			};

			class GRADIDOBLOCKCHAIN_EXPORT TransactionValidationForbiddenSignException : public TransactionValidationException
			{
			public:
				explicit TransactionValidationForbiddenSignException(memory::ConstBlockPtr forbiddenPubkey) noexcept;
				~TransactionValidationForbiddenSignException();

				std::string getFullString() const noexcept;
				rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;
			protected:
				memory::ConstBlockPtr mForbiddenPubkey;

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
				explicit TransactionValidationRequiredSignMissingException(const std::vector<memory::ConstBlockPtr>& missingPublicKeys) noexcept;

			protected:
				std::vector<std::string> mMissingPublicKeysHex;
			};

			class GRADIDOBLOCKCHAIN_EXPORT PairingTransactionNotMatchException : public TransactionValidationException
			{
			public:
				explicit PairingTransactionNotMatchException(
					const char* what,
					memory::ConstBlockPtr serializedTransaction,
					memory::ConstBlockPtr serializedPairingTransaction
				) noexcept;

				std::string getFullString() const noexcept;

			protected:
				std::shared_ptr<const data::GradidoTransaction> mTransaction;
				std::shared_ptr<const data::GradidoTransaction> mPairingTransaction;
			};

			class GRADIDOBLOCKCHAIN_EXPORT AddressAlreadyExistException : public TransactionValidationException
			{
			public:
				explicit AddressAlreadyExistException(const char* what, const std::string& addressHex, data::AddressType addressType) noexcept;

				std::string getFullString() const noexcept;
				rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

			protected:
				std::string mAddressHex;
				data::AddressType mAddressType;
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
				explicit WrongAddressTypeException(const char* what, data::AddressType type, memory::ConstBlockPtr pubkey) noexcept;

				std::string getFullString() const noexcept;
				rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

			protected:
				data::AddressType mType;
				memory::ConstBlockPtr mPubkey;
			};
		}
	}
}
#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_EXCEPTIONS_H