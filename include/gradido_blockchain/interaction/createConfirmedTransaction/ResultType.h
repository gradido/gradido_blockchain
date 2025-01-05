#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_RESULT_TYPE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_RESULT_TYPE_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/types.h"

namespace gradido {
    namespace interaction {
        namespace createConfirmedTransaction {
            enum class ResultType {
                NONE,
                ADDED,
                ALREADY_EXIST,
                INVALID,
                INVALID_PREVIOUS_TRANSACTION_IS_YOUNGER
            };
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_CONFIRMED_TRANSACTION_RESULT_TYPE_H