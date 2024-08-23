#ifndef __GRADIDO_BLOCKCHAIN_TRANSACTION_BODY_BUILDER_H
#define __GRADIDO_BLOCKCHAIN_TRANSACTION_BODY_BUILDER_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/data/TransactionBody.h"

namespace mem = memory;

namespace gradido {
	class GRADIDOBLOCKCHAIN_EXPORT TransactionBodyBuilder
	{
	public:
		TransactionBodyBuilder();
		~TransactionBodyBuilder();
		void reset();
		std::unique_ptr<data::TransactionBody> build();

		inline TransactionBodyBuilder& setDeferredTransfer(data::GradidoTransfer transactionTransfer, Timepoint timeout);
		TransactionBodyBuilder& setDeferredTransfer(std::unique_ptr<data::GradidoDeferredTransfer> deferredTransfer);

		inline TransactionBodyBuilder& setCommunityFriendsUpdate(bool colorFusion);
		TransactionBodyBuilder& setCommunityFriendsUpdate(std::unique_ptr<data::CommunityFriendsUpdate> communityFriendsUpdate);

		inline TransactionBodyBuilder& setRegisterAddress(
			mem::ConstBlockPtr userPubkey,
			data::AddressType type,
			mem::ConstBlockPtr nameHash = nullptr,
			mem::ConstBlockPtr accountPubkey = nullptr
		);
		TransactionBodyBuilder& setRegisterAddress(std::unique_ptr<data::RegisterAddress> registerAddress);

		inline TransactionBodyBuilder& setTransactionCreation(data::TransferAmount recipient, Timepoint targetDate);
		TransactionBodyBuilder& setTransactionCreation(std::unique_ptr<data::GradidoCreation> creation);

		inline TransactionBodyBuilder& setTransactionTransfer(data::TransferAmount sender, mem::ConstBlockPtr recipientPubkey);
		TransactionBodyBuilder& setTransactionTransfer(std::unique_ptr<data::GradidoTransfer> transfer);

		inline TransactionBodyBuilder& setCommunityRoot(
			mem::ConstBlockPtr pubkey,
			mem::ConstBlockPtr gmwPubkey,
			mem::ConstBlockPtr aufPubkey
		);
		TransactionBodyBuilder& setCommunityRoot(std::unique_ptr<data::CommunityRoot> communityRoot);

		//! if not called, time from calling TransactionBodyBuilder Constructor will be taken
		//! \param createAt timestamp when transaction where created
		inline TransactionBodyBuilder& setCreatedAt(Timepoint createdAt);
		inline TransactionBodyBuilder& setMemo(std::string_view memo);
		inline TransactionBodyBuilder& setVersionNumber(std::string_view versionNumber);

		//! LOCAL is default value, if this function isn't called
		inline TransactionBodyBuilder& setCrossGroupType(data::CrossGroupType type);

		//! only needed for cross group transactions
		inline TransactionBodyBuilder& setOtherGroup(std::string_view otherGroup);
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

	// ******************* inline declared functions *******************
	TransactionBodyBuilder& TransactionBodyBuilder::setDeferredTransfer(
		data::GradidoTransfer transactionTransfer,
		Timepoint timeout
	)
	{
		return setDeferredTransfer(
			std::make_unique<data::GradidoDeferredTransfer>(
				transactionTransfer,
				timeout
			)
		);
	}
	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityFriendsUpdate(bool colorFusion)
	{
		return setCommunityFriendsUpdate(
			std::make_unique<data::CommunityFriendsUpdate>(
				colorFusion
			)
		);
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setRegisterAddress(
		mem::ConstBlockPtr userPubkey,
		data::AddressType type,
		mem::ConstBlockPtr nameHash/* = nullptr*/,
		mem::ConstBlockPtr accountPubkey/* = nullptr*/
	) 
	{
		return setRegisterAddress(
			std::make_unique<data::RegisterAddress>(
				type,
				1,
				userPubkey,
				nameHash,
				accountPubkey
			)
		);
	};
	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionCreation(data::TransferAmount recipient, Timepoint targetDate) 
	{
		return setTransactionCreation(
			std::make_unique<data::GradidoCreation>(
				recipient, 
				targetDate
			)
		);
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setTransactionTransfer(
		data::TransferAmount sender,
		mem::ConstBlockPtr recipientPubkey
	) 
	{
		return setTransactionTransfer(
			std::make_unique<data::GradidoTransfer>(
				sender, 
				recipientPubkey
			)
		);
	}
	TransactionBodyBuilder& TransactionBodyBuilder::setCommunityRoot(
		mem::ConstBlockPtr pubkey,
		mem::ConstBlockPtr gmwPubkey,
		mem::ConstBlockPtr aufPubkey
	) {
		return setCommunityRoot(
			std::make_unique<data::CommunityRoot>(
				pubkey,
				gmwPubkey,
				aufPubkey
			)
		);
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCreatedAt(Timepoint createdAt) {
		mBody->mCreatedAt = data::Timestamp(createdAt);
		return *this;
	}
	TransactionBodyBuilder& TransactionBodyBuilder::setMemo(std::string_view memo) {
		mBody->mMemo = memo;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setVersionNumber(std::string_view versionNumber) {
		mBody->mVersionNumber = versionNumber;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setCrossGroupType(data::CrossGroupType type) {
		mBody->mType = type;
		return *this;
	}

	TransactionBodyBuilder& TransactionBodyBuilder::setOtherGroup(std::string_view otherGroup) {
		mBody->mOtherGroup = otherGroup;
		return *this;
	}

}

#endif //__GRADIDO_BLOCKCHAIN_TRANSACTION_BODY_BUILDER_H