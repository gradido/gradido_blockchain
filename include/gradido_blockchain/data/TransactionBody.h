#ifndef __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H
#define __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H

#include "CommunityFriendsUpdate.h"
#include "CommunityRoot.h"
#include "CrossGroupType.h"
#include "EncryptedMemo.h"
#include "GradidoCreation.h"
#include "GradidoDeferredTransfer.h"
#include "GradidoRedeemDeferredTransfer.h"
#include "GradidoTimeoutDeferredTransfer.h"
#include "RegisterAddress.h"
#include "Timestamp.h"
#include "TransactionType.h"

#include <optional>
#include <variant>

struct grdw_transaction_body;

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
			TransactionBody() : mType(CrossGroupType::LOCAL), mTransactionType(TransactionType::NONE), mCommunityIdIndex(0) {}
			TransactionBody(
				Timepoint createdAt,
				uint32_t communityIdIndex,
				CrossGroupType type = CrossGroupType::LOCAL,
				std::optional<uint32_t> otherCommunityIdIndex = std::nullopt
			) : mCreatedAt(createdAt), mType(type), mCommunityIdIndex(communityIdIndex), mOtherCommunityIdIndex(otherCommunityIdIndex) {};

			~TransactionBody() {}

			static std::shared_ptr<const TransactionBody> fromGrdwTransactionBody(grdw_transaction_body* grdw_body, uint32_t communityIdIndex);
			void toGrdw(grdw_transaction_body* grdw_body) const;

			inline bool isTransfer() const { return TransactionType::TRANSFER == mTransactionType; }
			inline bool isCreation() const { return TransactionType::CREATION == mTransactionType; }
			inline bool isCommunityFriendsUpdate() const { return TransactionType::COMMUNITY_FRIENDS_UPDATE == mTransactionType; }
			inline bool isRegisterAddress() const { return TransactionType::REGISTER_ADDRESS == mTransactionType; }
			inline bool isDeferredTransfer() const { return TransactionType::DEFERRED_TRANSFER == mTransactionType; }
			inline bool isCommunityRoot() const { return TransactionType::COMMUNITY_ROOT == mTransactionType; }
			inline bool isRedeemDeferredTransfer() const { return TransactionType::REDEEM_DEFERRED_TRANSFER == mTransactionType; }
			inline bool isTimeoutDeferredTransfer() const { return TransactionType::TIMEOUT_DEFERRED_TRANSFER == mTransactionType; }
			inline TransactionType getTransactionType() const { return mTransactionType; }

			bool isPairing(const TransactionBody& other) const;
			bool isInvolved(const memory::Block& publicKey) const;
			//! throw if not exist on this transaction type
			const TransferAmount& getTransferAmount() const;
			bool hasTransferAmount() const;

			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			inline const std::vector<EncryptedMemo>& getMemos() const { return mMemos; }
			inline Timestamp getCreatedAt() const { return mCreatedAt; }
			inline CrossGroupType getType() const { return mType; }
			inline uint32_t getCommunityIdIndex() const { return mCommunityIdIndex; }
			inline std::optional<uint32_t> getOtherCommunityIdIndex() const { return mOtherCommunityIdIndex; }

			inline std::shared_ptr<const GradidoTransfer> getTransfer() const { if (!isTransfer()) return nullptr; return std::get<1>(mSpecific); }
			inline std::shared_ptr<const GradidoCreation> getCreation() const { if (!isCreation()) return nullptr; return std::get<2>(mSpecific); }
			inline std::shared_ptr<const CommunityFriendsUpdate> getCommunityFriendsUpdate() const { if (!isCommunityFriendsUpdate()) return nullptr; return std::get<3>(mSpecific); }
			inline std::shared_ptr<const RegisterAddress> getRegisterAddress() const { if (!isRegisterAddress()) return nullptr; return std::get<4>(mSpecific); }
			inline std::shared_ptr<const GradidoDeferredTransfer> getDeferredTransfer() const { if (!isDeferredTransfer()) return nullptr; return std::get<5>(mSpecific); }
			inline std::shared_ptr<const CommunityRoot> getCommunityRoot() const { if (!isCommunityRoot()) return nullptr; return std::get<6>(mSpecific); }
			inline std::shared_ptr<const GradidoRedeemDeferredTransfer> getRedeemDeferredTransfer() const { if (!isRedeemDeferredTransfer()) return nullptr; return std::get<7>(mSpecific); }
			inline std::shared_ptr<const GradidoTimeoutDeferredTransfer> getTimeoutDeferredTransfer() const { if (!isTimeoutDeferredTransfer()) return nullptr; return std::get<8>(mSpecific); }

		protected:
			std::vector<EncryptedMemo>				mMemos;
			Timestamp													mCreatedAt;
			CrossGroupType										mType;
			TransactionType										mTransactionType;
			uint32_t													mCommunityIdIndex;
			std::optional<uint32_t> 					mOtherCommunityIdIndex;			
			using Specific = std::variant<
				std::monostate,
				std::shared_ptr<GradidoTransfer>,        
				std::shared_ptr<GradidoCreation>,        
				std::shared_ptr<CommunityFriendsUpdate>,
				std::shared_ptr<RegisterAddress>,        
				std::shared_ptr<GradidoDeferredTransfer>,
				std::shared_ptr<CommunityRoot>,          
				std::shared_ptr<GradidoRedeemDeferredTransfer>, 
				std::shared_ptr<GradidoTimeoutDeferredTransfer>
			>;
			Specific mSpecific;
		};

		typedef std::shared_ptr<const TransactionBody> ConstTransactionBodyPtr;
	}
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H