#ifndef __GRADIDO_BLOCKCHAIN_TRANSACTION_BODY_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_TRANSACTION_BODY_BUILDER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/TransactionBody.h"

namespace gradido {
	class GRADIDOBLOCKCHAIN_EXPORT TransactionBodyBuilder
	{
	public:
		TransactionBodyBuilder();
		~TransactionBodyBuilder();
		void reset();
		std::unique_ptr<data::TransactionBody> build();

		TransactionBodyBuilder& setDeferredTransfer(
			memory::ConstBlockPtr senderPubkey,
			const std::string& amountGddCent,
			const std::string& communityId,
			memory::ConstBlockPtr recipientPubkey,
			Timepoint timeout
		);
		TransactionBodyBuilder& setDeferredTransfer(
			std::unique_ptr<data::GradidoTransfer> transactionTransfer,
			Timepoint timeout
		);
		TransactionBodyBuilder& setDeferredTransfer(std::unique_ptr<data::GradidoDeferredTransfer> deferredTransfer);

		TransactionBodyBuilder& setCommunityFriendsUpdate(bool colorFusion);
		TransactionBodyBuilder& setCommunityFriendsUpdate(std::unique_ptr<data::CommunityFriendsUpdate> communityFriendsUpdate);

		TransactionBodyBuilder& setRegisterAddress(
			memory::ConstBlockPtr userPubkey,
			data::AddressType type,
			memory::ConstBlockPtr nameHash = nullptr,
			memory::ConstBlockPtr accountPubkey = nullptr
		);
		TransactionBodyBuilder& setRegisterAddress(std::unique_ptr<data::RegisterAddress> registerAddress);

		TransactionBodyBuilder& setTransactionCreation(
			memory::ConstBlockPtr recipientPubkey,
			const std::string& amountGddCent,
			Timepoint targetDate
		);
		TransactionBodyBuilder& setTransactionCreation(
			const data::TransferAmount& recipient,
			Timepoint targetDate
		);
		TransactionBodyBuilder& setTransactionCreation(
			std::unique_ptr<data::GradidoCreation> creation
		);

		TransactionBodyBuilder& setTransactionTransfer(
			memory::ConstBlockPtr senderPubkey,
			const std::string& amountGddCent,
			const std::string& communityId,
			memory::ConstBlockPtr recipientPubkey
		);
		TransactionBodyBuilder& setTransactionTransfer(std::unique_ptr<data::GradidoTransfer> transfer);

		TransactionBodyBuilder& setCommunityRoot(
			memory::ConstBlockPtr pubkey,
			memory::ConstBlockPtr gmwPubkey,
			memory::ConstBlockPtr aufPubkey
		);
		TransactionBodyBuilder& setCommunityRoot(std::unique_ptr<data::CommunityRoot> communityRoot);

		//! if not called, time from calling TransactionBodyBuilder Constructor will be taken
		inline TransactionBodyBuilder& setCreatedAt(Timepoint createdAt) {
			mBody->mCreatedAt = data::Timestamp(createdAt);			
			return *this;
		}
		inline TransactionBodyBuilder& setMemo(std::string_view memo) {
			mBody->mMemo = memo;
			return *this;
		}

		inline TransactionBodyBuilder& setVersionNumber(std::string_view versionNumber) {
			mBody->mVersionNumber = versionNumber;
			return *this;
		}

		//! LOCAL is default value, if this function isn't called
		inline TransactionBodyBuilder& setCrossGroupType(data::CrossGroupType type) {
			mBody->mType = type;
			return *this;
		}

		//! only needed for cross group transactions
		inline TransactionBodyBuilder& setOtherGroup(std::string_view otherGroup) {
			mBody->mOtherGroup = otherGroup;
			return *this;
		}
	protected:
		std::unique_ptr<data::TransactionBody> mBody;
		bool mSpecificTransactionChoosen;
	};

	// *******************  Exceptions ****************************
	class TransactionBodyBuilderException : public GradidoBlockchainException
	{
	public:
		explicit TransactionBodyBuilderException(const char* what) noexcept
			: GradidoBlockchainException(what) {}

		std::string getFullString() const {
			return what();
		}
	};
}

#endif //__GRADIDO_BLOCKCHAIN_TRANSACTION_BODY_BUILDER_H