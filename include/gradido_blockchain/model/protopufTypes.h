#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TYPES_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TYPES_H

/*
* Using protopuf, a header only library for serialisation and deserialisation of protobuf messages
* definitions of gradido transaction protofol in protopuf format
*/

#include "protopuf/message.h"

using namespace pp;

namespace model {
    namespace protopuf {

        using SignaturePair = message<
            bytes_field<"pubkey", 1, pp::singular, std::vector<std::vector<std::byte>>>,
            bytes_field<"signature", 2, pp::singular, std::vector<std::vector<std::byte>>>
        >;

        using SignatureMap = message<
            message_field<"sig_pair", 1, SignaturePair, repeated>
        >;

        using Timestamp = message<
            int64_field<"seconds", 1>,
            int32_field<"nanos", 2>
        >;

        using TimestampSeconds = message<
            int64_field<"seconds", 1>
        >;

        using TransferAmount = message<
            bytes_field<"pubkey", 1>,
            string_field<"amount", 2>,
            string_field<"community_id", 3>
        >;

        using GradidoTransfer = message<
            message_field<"sender", 1, TransferAmount>,
            bytes_field<"recipient", 2>
        >;

        using GradidoDeferredTransfer = message<
            message_field<"transfer", 1, GradidoTransfer>,
            message_field<"timeout", 2, TimestampSeconds>
        >;



        enum class CrossGroupType {
            LOCAL = 0,
            INBOUND = 1,
            OUTBOUND = 2,
            CROSS = 3
        };

        using GradidoCreation = message<
            message_field<"recipient", 1, TransferAmount>,
            message_field<"target_date", 3, TimestampSeconds>
        >;

        using CommunityFriendsUpdate = message<
            bool_field<"color_fusion", 1>
        >;

        enum class AddressType {
            NONE = 0, // if no address was found
            COMMUNITY_HUMAN = 1, // creation account for human
            COMMUNITY_GMW = 2, // community public budget account
            COMMUNITY_AUF = 3, // community compensation and environment founds account
            COMMUNITY_PROJECT = 4, // no creations allowed
            SUBACCOUNT = 5, // no creations allowed
            CRYPTO_ACCOUNT = 6 // user control his keys, no creations
        };

        using RegisterAddress = message<
            bytes_field<"user_pubkey", 1>,
            enum_field<"address_type", 2, AddressType>,
            bytes_field<"name_hash", 3>,
            bytes_field<"account_pubkey", 4>,
            uint32_field<"derivation_index", 5>
        >;

        using CommunityRoot = message <
            bytes_field<"pubkey", 1>,
            bytes_field<"gmw_pubkey", 2>,
            bytes_field<"auf_pubkey", 3>
        >;

        using TransactionBody = message<
            string_field<"memo", 1>,
            message_field<"created_at", 2, Timestamp>,
            string_field<"version_number", 3>,
            enum_field<"type", 4, CrossGroupType>,
            string_field<"other_group", 5>,

            // oneof isn't supported from protopuf
            // oneof data
            message_field<"transfer", 6, GradidoTransfer>,
            message_field<"creation", 7, GradidoCreation>,
            message_field<"community_friends_update", 8, CommunityFriendsUpdate>,
            message_field<"register_address", 9, RegisterAddress>,
            message_field<"deferred_transfer", 10, GradidoDeferredTransfer>,
            message_field<"community_root", 11, CommunityRoot>
        >;

        using GradidoTransaction = message<
            message_field<"sig_map", 1, SignatureMap>,
            bytes_field<"body_bytes", 2>,
            bytes_field<"parent_message_id", 3>
        >;

        using ConfirmedTransaction = message<
            uint64_field<"id", 1>,
            message_field<"transaction", 2, GradidoTransaction>,
            message_field<"confirmed_at", 3, TimestampSeconds>,
            string_field<"version_number", 4>,
            bytes_field<"running_hash", 5>,
            bytes_field<"message_id", 6>,
            string_field<"account_balance", 7>
        >;
    }
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TYPES_H