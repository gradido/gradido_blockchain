#ifndef __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H
#define __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H

#include "EncryptedMemo.h"
#include "GradidoDeferredTransfer.h"
#include "GradidoCreation.h"
#include "RegisterAddress.h"
#include "CommunityFriendsUpdate.h"
#include "CommunityRoot.h"
#include "GradidoRedeemDeferredTransfer.h"
#include "GradidoTimeoutDeferredTransfer.h"
#include "Timestamp.h"

#include "CrossGroupType.h"
#include "TransactionType.h"

namespace gradido {
	class GradidoTransactionBuilder;
	namespace interaction {
		namespace deserialize {
			class TransactionBodyRole;
		}
	}
	namespace data {
		class GRADIDOBLOCKCHAIN_EXPORT TransactionBody
		{
			friend GradidoTransactionBuilder;
			friend interaction::deserialize::TransactionBodyRole;
		public:
			TransactionBody() : mType(CrossGroupType::LOCAL) {}
			TransactionBody(
				Timepoint _createdAt,
				const std::string& _versionNumber,
				CrossGroupType _type = CrossGroupType::LOCAL,
				const std::string& _otherGroup = ""
			) : mCreatedAt(_createdAt), mVersionNumber(_versionNumber), mType(_type), mOtherGroup(_otherGroup) {};

			~TransactionBody() {}

			inline bool isTransfer() const { return static_cast<bool>(mTransfer); }
			inline bool isCreation() const { return static_cast<bool>(mCreation); }
			inline bool isCommunityFriendsUpdate() const { return static_cast<bool>(mCommunityFriendsUpdate); }
			inline bool isRegisterAddress() const { return static_cast<bool>(mRegisterAddress); }
			inline bool isDeferredTransfer() const { return static_cast<bool>(mDeferredTransfer); }
			inline bool isCommunityRoot() const { return static_cast<bool>(mCommunityRoot); }
			inline bool isRedeemDeferredTransfer() const { return static_cast<bool>(mRedeemDeferredTransfer); }
			inline bool isTimeoutDeferredTransfer() const { return static_cast<bool>(mTimeoutDeferredTransfer); }
			TransactionType getTransactionType() const;

			bool isPairing(const TransactionBody& other) const;
			bool isInvolved(const memory::Block& publicKey) const;
			//! return nullptr, if transaction type doesn't have a transfer amount
			const TransferAmount& getTransferAmount() const;

			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			inline const std::vector<EncryptedMemo>& getMemos() const { return mMemos; }
			inline Timestamp getCreatedAt() const { return mCreatedAt; }
			inline const std::string& getVersionNumber() const { return mVersionNumber; }
			inline CrossGroupType getType() const { return mType; }
			inline const std::string& getOtherGroup() const { return mOtherGroup; }

			inline std::shared_ptr<const GradidoTransfer> getTransfer() const { return mTransfer; }
			inline std::shared_ptr<const GradidoCreation> getCreation() const { return mCreation; }
			inline std::shared_ptr<const CommunityFriendsUpdate> getCommunityFriendsUpdate() const { return mCommunityFriendsUpdate; }
			inline std::shared_ptr<const RegisterAddress> getRegisterAddress() const { return mRegisterAddress; }
			inline std::shared_ptr<const GradidoDeferredTransfer> getDeferredTransfer() const { return mDeferredTransfer; }
			inline std::shared_ptr<const CommunityRoot> getCommunityRoot() const { return mCommunityRoot; }
			inline std::shared_ptr<const GradidoRedeemDeferredTransfer> getRedeemDeferredTransfer() const { return mRedeemDeferredTransfer; }
			inline std::shared_ptr<const GradidoTimeoutDeferredTransfer> getTimeoutDeferredTransfer() const { return mTimeoutDeferredTransfer; }

		protected:
			std::vector<EncryptedMemo>				mMemos;
			Timestamp								mCreatedAt;
			std::string								mVersionNumber;
			CrossGroupType							mType;
			std::string								mOtherGroup;

			std::shared_ptr<GradidoTransfer>        mTransfer;
			std::shared_ptr<GradidoCreation>        mCreation;
			std::shared_ptr<CommunityFriendsUpdate> mCommunityFriendsUpdate;
			std::shared_ptr<RegisterAddress>        mRegisterAddress;
			std::shared_ptr<GradidoDeferredTransfer> mDeferredTransfer;
			std::shared_ptr<CommunityRoot>          mCommunityRoot;
			std::shared_ptr<GradidoRedeemDeferredTransfer> mRedeemDeferredTransfer;
			std::shared_ptr<GradidoTimeoutDeferredTransfer> mTimeoutDeferredTransfer;

		};

		typedef std::shared_ptr<const TransactionBody> ConstTransactionBodyPtr;
	}
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H