#include "gradido_blockchain/interaction/deserialize/HieroAccountIdRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"

namespace gradido {
    namespace interaction {
        namespace deserialize {
            HieroAccountIdRole::HieroAccountIdRole(const HieroAccountIdMessage& hieroAccountId)
            {
                int64_t shardNum = 0;
                int64_t realmNum = 0;
                if (hieroAccountId["shardNum"_f].has_value()) {
                    shardNum = hieroAccountId["shardNum"_f].value();
                }
                if (hieroAccountId["realmNum"_f].has_value()) {
                    realmNum = hieroAccountId["realmNum"_f].value();
                }
                if (hieroAccountId["accountNum"_f].has_value()) {
                    mAccountId = hiero::AccountId(shardNum, realmNum, hieroAccountId["accountNum"_f].value());
                }
                else if (hieroAccountId["alias"_f].has_value()) {
                    auto alias = memory::Block(hieroAccountId["alias"_f].value());
                    mAccountId = hiero::AccountId(shardNum, realmNum, std::move(alias));
                }
                throw MissingMemberException("either accountNum or alias must be set", "hiero::AccountId");
            }

            HieroAccountIdRole::~HieroAccountIdRole()
            {

            }
        }
    }
}