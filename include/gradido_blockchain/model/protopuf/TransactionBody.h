#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_TRANSACTION_BODY_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_TRANSACTION_BODY_H

#include "GradidoTransfer.h"
#include "GradidoCreation.h"
#include "CommunityFriendsUpdate.h"
#include "RegisterAddress.h"
#include "GradidoDeferredTransfer.h"
#include "CommunityRoot.h"
#include "Timestamp.h"

#include "gradido_blockchain/model/CrossGroupType.h"

namespace model {
	namespace protopuf {
        using TransactionBodyMessage = message<
            string_field<"memo", 1, singular, std::vector<StringCachedAlloc>>,
            message_field<"created_at", 2, TimestampMessage>,
            string_field<"version_number", 3, singular, std::vector<StringCachedAlloc>>,
            enum_field<"type", 4, CrossGroupType>,
            string_field<"other_group", 5, singular, std::vector<StringCachedAlloc>>,

            // oneof isn't supported from protopuf
            // oneof data
            message_field<"transfer", 6, GradidoTransferMessage>,
            message_field<"creation", 7, GradidoCreationMessage>,
            message_field<"community_friends_update", 8, CommunityFriendsUpdateMessage>,
            message_field<"register_address", 9, RegisterAddressMessage>,
            message_field<"deferred_transfer", 10, GradidoDeferredTransferMessage>,
            message_field<"community_root", 11, CommunityRootMessage>
        >;

        class TransactionBody 
        {
        public:
            TransactionBody(const TransactionBodyMessage& data);
            TransactionBody(
                const std::string& memo, 
                Timepoint createdAt,
                const std::string& versionNumber,
                CrossGroupType type = CrossGroupType::LOCAL, 
                const std::string& otherGroup = ""
            );

            inline void setTransfer(std::shared_ptr<GradidoTransfer> transfer) { mTransfer = transfer; }
            inline void setCreation(std::shared_ptr<GradidoCreation> creation) { mCreation = creation; }
            inline void setCommunityFriendsUpdate(std::shared_ptr<CommunityFriendsUpdate> communityFriendsUpdate) {
                mCommunityFriendsUpdate = communityFriendsUpdate;
            }
            inline void setRegisterAddress(std::shared_ptr<RegisterAddress> registerAddress) { mRegisterAddress = registerAddress; }
            inline void setDeferredTransfer(std::shared_ptr<GradidoDeferredTransfer> deferredTransfer) { mDeferredTransfer = deferredTransfer; }
            inline void setCommunityRoot(std::shared_ptr<CommunityRoot> communityRoot) { mCommunityRoot = communityRoot;}

            inline std::shared_ptr<GradidoTransfer> getTransfer() const { return mTransfer; }
            inline std::shared_ptr<GradidoCreation> getCreation() const { return mCreation; }
            inline std::shared_ptr<CommunityFriendsUpdate> getCommunityFriendsUpdate() const {
                return mCommunityFriendsUpdate;
            }
            inline std::shared_ptr<RegisterAddress> getRegisterAddress() const { return mRegisterAddress;}
            inline std::shared_ptr<GradidoDeferredTransfer> getDeferredTransfer() const { return mDeferredTransfer; }
            inline std::shared_ptr<CommunityRoot> getCommunityRoot() const { return mCommunityRoot; }

            inline bool isTransfer() const { return static_cast<bool>(mTransfer); }
            inline bool isCreation() const { return static_cast<bool>(mCreation); }
            inline bool isCommunityFriendsUpdate() const { return static_cast<bool>(mCommunityFriendsUpdate); }
            inline bool isRegisterAddress() const { return static_cast<bool>(mRegisterAddress); }
            inline bool isDeferredTransfer() const { return static_cast<bool>(mDeferredTransfer); }
            inline bool isCommunityRoot() const { return static_cast<bool>(mCommunityRoot); }
            
            ~TransactionBody() {}
        protected:
            std::string    mMemo;
            Timestamp      mCreatedAt;
            std::string    mVersionNumber;
            CrossGroupType mType;
            std::string    mOtherGroup;

            std::shared_ptr<GradidoTransfer>        mTransfer;
            std::shared_ptr<GradidoCreation>        mCreation;
            std::shared_ptr<CommunityFriendsUpdate> mCommunityFriendsUpdate;
            std::shared_ptr<RegisterAddress>        mRegisterAddress;
            std::shared_ptr<GradidoDeferredTransfer> mDeferredTransfer;
            std::shared_ptr<CommunityRoot>          mCommunityRoot;

        };

	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOBUF_TRANSACTION_BODY_H