#ifndef __GRADIDO_BLOCKCHAIN_DATA_MEMO_KEY_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_MEMO_KEY_TYPE_H

namespace gradido {
    namespace data {

        enum class MemoKeyType {
            SHARED_SECRET = 0,
            COMMUNITY_SECRET = 1,
            PLAIN = 2
        };
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_MEMO_KEY_TYPE_H