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

#include "compact/CommunityRootTx.h"

#include <optional>

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
			TransactionBody() : mType(CrossGroupType::LOCAL), mCommunityIdIndex(0), mTransactionType(TransactionType::NONE) {}
			TransactionBody(
				Timepoint createdAt,
				const std::string& versionNumber,
				uint32_t communityIdIndex,
				CrossGroupType type = CrossGroupType::LOCAL,
				std::optional<uint32_t> otherCommunityIdIndex = std::nullopt
			) : mCreatedAt(createdAt), mVersionNumber(versionNumber), mType(type), 
				mCommunityIdIndex(communityIdIndex), mOtherCommunityIdIndex(otherCommunityIdIndex),
				mTransactionType(TransactionType::NONE) {}

			~TransactionBody() {}

			inline bool isTransfer() const { return static_cast<bool>(mTransfer); }
			inline bool isCreation() const { return static_cast<bool>(mCreation); }
			inline bool isCommunityFriendsUpdate() const { return static_cast<bool>(mCommunityFriendsUpdate); }
			inline bool isRegisterAddress() const { return static_cast<bool>(mRegisterAddress); }
			inline bool isDeferredTransfer() const { return static_cast<bool>(mDeferredTransfer); }
			inline bool isCommunityRoot() const { return TransactionType::COMMUNITY_ROOT == mTransactionType; }
			inline bool isRedeemDeferredTransfer() const { return static_cast<bool>(mRedeemDeferredTransfer); }
			inline bool isTimeoutDeferredTransfer() const { return static_cast<bool>(mTimeoutDeferredTransfer); }
			TransactionType getTransactionType() const;

			bool isPairing(const TransactionBody& other) const;
			bool isInvolved(const memory::Block& publicKey) const;
			//! throw if not exist on this transaction type
			const TransferAmount& getTransferAmount() const;
			bool hasTransferAmount() const;

			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			inline const std::vector<EncryptedMemo>& getMemos() const { return mMemos; }
			inline Timestamp getCreatedAt() const { return mCreatedAt; }
			inline const std::string& getVersionNumber() const { return mVersionNumber; }
			inline CrossGroupType getType() const { return mType; }
			inline uint32_t getCommunityIdIndex() const { return mCommunityIdIndex; }
			inline std::optional<uint32_t> getOtherCommunityIdIndex() const { return mOtherCommunityIdIndex; }

			inline std::shared_ptr<const GradidoTransfer> getTransfer() const { return mTransfer; }
			inline std::shared_ptr<const GradidoCreation> getCreation() const { return mCreation; }
			inline std::shared_ptr<const CommunityFriendsUpdate> getCommunityFriendsUpdate() const { return mCommunityFriendsUpdate; }
			inline std::shared_ptr<const RegisterAddress> getRegisterAddress() const { return mRegisterAddress; }
			inline std::shared_ptr<const GradidoDeferredTransfer> getDeferredTransfer() const { return mDeferredTransfer; }
			//inline std::shared_ptr<const CommunityRoot> getCommunityRoot() const { return mCommunityRoot; }
			inline std::shared_ptr<const GradidoRedeemDeferredTransfer> getRedeemDeferredTransfer() const { return mRedeemDeferredTransfer; }
			inline std::shared_ptr<const GradidoTimeoutDeferredTransfer> getTimeoutDeferredTransfer() const { return mTimeoutDeferredTransfer; }

			inline std::optional<compact::CommunityRootTx> getCommunityRoot() const;

		protected:
			std::vector<EncryptedMemo>				mMemos;
			Timestamp													mCreatedAt;
			std::string												mVersionNumber;
			CrossGroupType										mType;
			uint32_t													mCommunityIdIndex;
			std::optional<uint32_t> 					mOtherCommunityIdIndex;

			TransactionType										mTransactionType;
			union {
				compact::CommunityRootTx communityRoot;
			} mSpecific;

			std::shared_ptr<GradidoTransfer>        mTransfer;
			std::shared_ptr<GradidoCreation>        mCreation;
			std::shared_ptr<CommunityFriendsUpdate> mCommunityFriendsUpdate;
			std::shared_ptr<RegisterAddress>        mRegisterAddress;
			std::shared_ptr<GradidoDeferredTransfer> mDeferredTransfer;
			// std::shared_ptr<CommunityRoot>          mCommunityRoot;
			std::shared_ptr<GradidoRedeemDeferredTransfer> mRedeemDeferredTransfer;
			std::shared_ptr<GradidoTimeoutDeferredTransfer> mTimeoutDeferredTransfer;

		};


		std::optional<compact::CommunityRootTx> TransactionBody::getCommunityRoot() const 
		{ 
			if (isCommunityRoot()) { return mSpecific.communityRoot; }
			return std::nullopt;
		}

		typedef std::shared_ptr<const TransactionBody> ConstTransactionBodyPtr;
	}
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H