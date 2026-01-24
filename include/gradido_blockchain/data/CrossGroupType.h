#ifndef __GRADIDO_BLOCKCHAIN_DATA_CROSS_GROUP_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_CROSS_GROUP_TYPE_H

namespace gradido {
        namespace data {
            enum class CrossGroupType: uint8_t {
                LOCAL = 0,
                INBOUND = 1,
                OUTBOUND = 2,
                CROSS = 3
            };
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_CROSS_GROUP_TYPE_H