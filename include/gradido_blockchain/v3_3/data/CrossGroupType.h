#ifndef __GRADIDO_BLOCKCHAIN_v3_3_DATA_CROSS_GROUP_TYPE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_DATA_CROSS_GROUP_TYPE_H

namespace gradido {
    namespace v3_3 {
        namespace data {

            enum class CrossGroupType {
                LOCAL = 0,
                INBOUND = 1,
                OUTBOUND = 2,
                CROSS = 3
            };
        }
    }
    
}
#endif //__GRADIDO_BLOCKCHAIN_v3_3_DATA_CROSS_GROUP_TYPE_H