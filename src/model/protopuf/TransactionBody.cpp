#include "gradido_blockchain/model/protopuf/TransactionBody.h"

namespace model {
	namespace protopuf {
        TransactionBody::TransactionBody(const TransactionBodyMessage& data)
            : mMemo(data["memo"_f].value_or("")), 
            mCreatedAt(data["created_at"_f].value()),
            mVersionNumber(data["version_number"_f].value()),
            mType(data["type"_f].value()),
            mOtherGroup(data["other_group"_f].value_or(""))
        {
            if (data["transfer"_f].has_value()) {
                mTransfer = std::make_shared<GradidoTransfer>(data["transfer"_f].value());
            }
            else if (data["creation"_f].has_value()) {
                mCreation = std::make_shared<GradidoCreation>(data["creation"_f].value());
            }
            else if (data["community_friends_update"_f].has_value()) {
                mCommunityFriendsUpdate = std::make_shared<CommunityFriendsUpdate>(data["community_friends_update"_f].value());
            }
            else if (data["register_address"_f].has_value()) {
                mRegisterAddress = std::make_shared<RegisterAddress>(data["register_address"_f].value());
            }
            else if (data["deferred_transfer"_f].has_value()) {
                mDeferredTransfer = std::make_shared<GradidoDeferredTransfer>(data["deferred_transfer"_f].has_value());
            } 
            else if (data["community_root"_f].has_value()) {
                mCommunityRoot = std::make_shared<CommunityRoot>(data["community_root"_f].has_value());
            }
        }
        TransactionBody::TransactionBody(
            const std::string& memo,
            Timepoint createdAt,
            const std::string& versionNumber,
            CrossGroupType type /* = CrossGroupType::LOCAL */,
            const std::string& otherGroup /* = "" */
        ) : mMemo(memo), mCreatedAt(createdAt), mVersionNumber(versionNumber), mType(type), mOtherGroup(otherGroup)
        {

        }
	}
}