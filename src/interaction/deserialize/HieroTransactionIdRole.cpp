#include "gradido_blockchain/interaction/deserialize/HieroAccountIdRole.h"
#include "gradido_blockchain/interaction/deserialize/HieroTransactionIdRole.h"
#include "gradido_blockchain/interaction/deserialize/TimestampRole.h"
#include "gradido_blockchain/interaction/deserialize/Exceptions.h"


namespace gradido {
    namespace interaction {
        namespace deserialize {
            HieroTransactionIdRole::HieroTransactionIdRole(const HieroTransactionIdMessage& hieroTransactionId)
            {
                if (!hieroTransactionId["transactionValidStart"_f].has_value() || 
                    !hieroTransactionId["accountID"_f].has_value()) {
                    throw MissingMemberException("transactionValidStart and/or accountID is missing", "hiero::TransactionId");
                }
                mTransactionId = hiero::TransactionId(
                    TimestampRole(hieroTransactionId["transactionValidStart"_f].value()),
                    HieroAccountIdRole(hieroTransactionId["accountID"_f].value())
                );
                if (hieroTransactionId["scheduled"_f].has_value() && hieroTransactionId["scheduled"_f].value()) {
                    mTransactionId.setScheduled();
                }
                if (hieroTransactionId["nonce"_f].has_value()) {
                    mTransactionId.setNonce(hieroTransactionId["nonce"_f].value());
                }
            }

            HieroTransactionIdRole::~HieroTransactionIdRole()
            {

            }
        }
    }
}