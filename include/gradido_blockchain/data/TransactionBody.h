#ifndef GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H
#define GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H

#include "CommunityFriendsUpdate.h"
#include "CommunityRoot.h"
#include "EncryptedMemo.h"
#include "GradidoCreation.h"
#include "GradidoDeferredTransfer.h"
#include "GradidoRedeemDeferredTransfer.h"
#include "GradidoTimeoutDeferredTransfer.h"
#include "RegisterAddress.h"
#include "Timestamp.h"

#include "compact/CommunityRootTx.h"
#include "compact/RegisterAddressTx.h"

#include "gradido_blockchain_core/types/cross_group.h"
#include "gradido_blockchain_core/types/transaction.h"

#include <optional>
#include <variant>

struct grdw_transaction_body;
struct grd_memory;

namespace gradido {
	class GradidoTransactionBuilder;
	namespace interaction::deserialize {
		class TransactionBodyRole;
	}
	namespace data {
		class GradidoTransfer;
		namespace compact {
			struct PublicKeyIndex;
		}

		class GRADIDOBLOCKCHAIN_EXPORT TransactionBody
		{
			friend GradidoTransactionBuilder;
			friend interaction::deserialize::TransactionBodyRole;
		public:
			TransactionBody() : mType(GRDT_CROSS_GROUP_LOCAL), mTransactionType(GRDT_TRANSACTION_NONE), mCommunityIdIndex(0) {}
			TransactionBody(
				Timepoint createdAt,
				uint32_t communityIdIndex,
				grdt_cross_group type = GRDT_CROSS_GROUP_LOCAL,
				std::optional<uint32_t> otherCommunityIdIndex = std::nullopt
			) : mCreatedAt(createdAt), mType(type), mTransactionType(GRDT_TRANSACTION_NONE), mCommunityIdIndex(communityIdIndex), mOtherCommunityIdIndex(otherCommunityIdIndex) {};

			~TransactionBody() {}

			static std::shared_ptr<const TransactionBody> fromGrdw(grdw_transaction_body* grdw_body, uint32_t communityIdIndex);
			void toGrdw(grd_memory* alloc, grdw_transaction_body* grdw_body) const;

			inline bool isTransfer() const { return GRDT_TRANSACTION_TRANSFER == mTransactionType; }
			inline bool isCreation() const { return GRDT_TRANSACTION_CREATION == mTransactionType; }
			inline bool isCommunityFriendsUpdate() const { return GRDT_TRANSACTION_COMMUNITY_FRIENDS_UPDATE == mTransactionType; }
			inline bool isRegisterAddress() const { return GRDT_TRANSACTION_REGISTER_ADDRESS == mTransactionType; }
			inline bool isDeferredTransfer() const { return GRDT_TRANSACTION_DEFERRED_TRANSFER == mTransactionType; }
			inline bool isCommunityRoot() const { return GRDT_TRANSACTION_COMMUNITY_ROOT == mTransactionType; }
			inline bool isRedeemDeferredTransfer() const { return GRDT_TRANSACTION_REDEEM_DEFERRED_TRANSFER == mTransactionType; }
			inline bool isTimeoutDeferredTransfer() const { return GRDT_TRANSACTION_TIMEOUT_DEFERRED_TRANSFER == mTransactionType; }
			inline grdt_transaction getTransactionType() const { return mTransactionType; }

			bool isPairing(const TransactionBody& other) const;
			[[deprecated("Replaced by isInvolved with compact::PublicKeyIndex")]]
			bool isInvolved(const memory::Block& publicKey) const;
			bool isInvolved(compact::PublicKeyIndex publicKeyIndex) const;
			//! throw if not exist on this transaction type
			const TransferAmount& getTransferAmount() const;
			bool hasTransferAmount() const;

			[[deprecated("Replaced by getInvolvedAddressIndices")]]
			std::vector<memory::ConstBlockPtr> getInvolvedAddresses() const;
			std::vector<compact::PublicKeyIndex> getInvolvedAddressIndices() const;

			inline const std::vector<EncryptedMemo>& getMemos() const { return mMemos; }
			inline Timestamp getCreatedAt() const { return mCreatedAt; }
			inline grdt_cross_group getType() const { return mType; }
			inline uint32_t getCommunityIdIndex() const { return mCommunityIdIndex; }
			inline std::optional<uint32_t> getOtherCommunityIdIndex() const { return mOtherCommunityIdIndex; }

			inline std::shared_ptr<const GradidoTransfer> getTransfer() const { if (!isTransfer()) return nullptr; return std::get<1>(mSpecific); }
			inline std::shared_ptr<const GradidoCreation> getCreation() const { if (!isCreation()) return nullptr; return std::get<2>(mSpecific); }
			inline std::shared_ptr<const CommunityFriendsUpdate> getCommunityFriendsUpdate() const { if (!isCommunityFriendsUpdate()) return nullptr; return std::get<3>(mSpecific); }
			inline std::shared_ptr<const GradidoDeferredTransfer> getDeferredTransfer() const { if (!isDeferredTransfer()) return nullptr; return std::get<5>(mSpecific); }
			inline std::shared_ptr<const GradidoRedeemDeferredTransfer> getRedeemDeferredTransfer() const { if (!isRedeemDeferredTransfer()) return nullptr; return std::get<7>(mSpecific); }
			inline std::shared_ptr<const GradidoTimeoutDeferredTransfer> getTimeoutDeferredTransfer() const { if (!isTimeoutDeferredTransfer()) return nullptr; return std::get<8>(mSpecific); }

			inline std::optional<compact::CommunityRootTx> getCommunityRoot() const;
			inline std::optional<compact::RegisterAddressTx> getRegisterAddress() const;

		protected:

			void fillFromGradidoTransfer(std::vector<compact::PublicKeyIndex>& publicKeys, const GradidoTransfer& transfer) const;

			std::vector<EncryptedMemo>				mMemos;
			Timestamp													mCreatedAt;
			grdt_cross_group									mType;
			grdt_transaction									mTransactionType;
			uint32_t													mCommunityIdIndex;
			std::optional<uint32_t> 					mOtherCommunityIdIndex;			
			using Specific = std::variant<
				std::monostate,
				std::shared_ptr<GradidoTransfer>,        
				std::shared_ptr<GradidoCreation>,        
				std::shared_ptr<CommunityFriendsUpdate>,
				compact::RegisterAddressTx,
				std::shared_ptr<GradidoDeferredTransfer>,
				compact::CommunityRootTx,
				std::shared_ptr<GradidoRedeemDeferredTransfer>, 
				std::shared_ptr<GradidoTimeoutDeferredTransfer>
			>;
			Specific mSpecific;
		};


		std::optional<compact::CommunityRootTx> TransactionBody::getCommunityRoot() const 
		{ 
			if (isCommunityRoot()) { return std::get<6>(mSpecific); }
			return std::nullopt;
		}

		std::optional<compact::RegisterAddressTx> TransactionBody::getRegisterAddress() const
		{
			if (isRegisterAddress()) { return std::get<4>(mSpecific); }
			return std::nullopt;
		}
		


		typedef std::shared_ptr<const TransactionBody> ConstTransactionBodyPtr;
	}
}
#endif //GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_BODY_H