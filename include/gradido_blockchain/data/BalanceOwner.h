#ifndef __GRADIDO_BLOCKCHAIN_DATA_BALANCE_OWNER_H
#define __GRADIDO_BLOCKCHAIN_DATA_BALANCE_OWNER_H

namespace gradido {
    namespace data {

        enum class BalanceOwner {
            SENDER = 0,
            RECIPIENT = 1
        };
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_BALANCE_OWNER_H