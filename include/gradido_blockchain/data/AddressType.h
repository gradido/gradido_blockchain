#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADDRESS_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADDRESS_TYPE_H

namespace gradido {
    namespace data {

        enum class AddressType {
            NONE = 0, // if no address was found
            COMMUNITY_HUMAN = 1, // creation account for human
            COMMUNITY_GMW = 2, // community public budget account
            COMMUNITY_AUF = 3, // community compensation and environment founds account
            COMMUNITY_PROJECT = 4, // no creations allowed
            SUBACCOUNT = 5, // no creations allowed
            CRYPTO_ACCOUNT = 6, // user control his keys, no creations
            DEFERRED_TRANSFER = 7 // special type, no need for register address
        };
    }
}
#endif //__GRADIDO_BLOCKCHAIN_DATA_ADDRESS_TYPE_H