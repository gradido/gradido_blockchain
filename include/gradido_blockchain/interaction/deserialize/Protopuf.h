#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_PROTOPUF_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_PROTOPUF_H

#include <string>
#include <bit>
#include "protopuf/message.h"

#include "gradido_blockchain/data/AddressType.h"
#include "gradido_blockchain/data/BalanceDerivationType.h"
#include "gradido_blockchain/data/CrossGroupType.h"
#include "gradido_blockchain/data/LedgerAnchor.h"
#include "gradido_blockchain/data/TransactionTriggerEventType.h"
#include "gradido_blockchain/data/MemoKeyType.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"

using namespace pp;
using namespace memory;
using namespace std;

namespace gradido {
    namespace interaction {
        namespace deserialize {

            //  ----------------  basic_types.proto -----------------------------------
            using AccountBalanceMessage = message<
                bytes_field <"pubkey", 1>,
                int64_field<"balance", 2>,
                string_field<"community_id", 3>
            >;

            using EncryptedMemoMessage = message<
                enum_field<"type", 1, data::MemoKeyType>,
                bytes_field<"memo", 2>
            >;

            using SignaturePairMessage = message<
                bytes_field<"pubkey", 1>,// singular, vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                bytes_field<"signature", 2>//, singular, vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
            >;

            using SignatureMapMessage = message<
                message_field<"sig_pair", 1, SignaturePairMessage, repeated>
            >;

            using TimestampMessage = message<
                int64_field<"seconds", 1>,
                int32_field<"nanos", 2>
            >;

            using TimestampSecondsMessage = message<
                int64_field<"seconds", 1>
            >;

            using DurationSecondsMessage = message<
                uint32_field<"seconds", 1>
            >;

            using TransferAmountMessage = message<
                bytes_field<"pubkey", 1>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                int64_field<"amount", 2>,// singular, std::vector<StringCachedAlloc>>,
                string_field<"community_id", 3>//, singular, std::vector<StringCachedAlloc>>
            >;

            // hiero
            using HieroAccountIdMessage = message<
                int64_field<"shardNum", 1>,
                int64_field<"realmNum", 2>,
                int64_field<"accountNum", 3>,
                bytes_field<"alias", 4>
            >;

            using HieroTopicIdMessage = message<
                int64_field<"shardNum", 1>,
                int64_field<"realmNum", 2>,
                int64_field<"topicNum", 3>
            >;

            using HieroTransactionIdMessage = message<
                message_field<"transactionValidStart", 1, TimestampMessage>,
                message_field<"accountID", 2, HieroAccountIdMessage>,
                bool_field<"scheduled", 3>,
                int32_field<"nonce", 4>
            >;

            //  ---------------- end   basic_types.proto   end -----------------------------------
            // 
            // community_friends_update.proto
            using CommunityFriendsUpdateMessage = message<
                bool_field<"color_fusion", 1>
            >;

            // community_root.proto
            using CommunityRootMessage = message <
                bytes_field<"pubkey", 1>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                bytes_field<"gmw_pubkey", 2>,//, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
                bytes_field<"auf_pubkey", 3>
            >;

            // gradido_creation.proto
            using GradidoCreationMessage = message<
                message_field<"recipient", 1, TransferAmountMessage>,
                message_field<"target_date", 3, TimestampSecondsMessage>
            >;

            // gradido_transfer.proto
            using GradidoTransferMessage = message<
                message_field<"sender", 1, TransferAmountMessage>,
                bytes_field<"recipient", 2>//, pp::singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
            >;

            using GradidoDeferredTransferMessage = message<
                message_field<"transfer", 1, GradidoTransferMessage>,
                message_field<"timeout_duration", 2, DurationSecondsMessage>
            >;

            using GradidoRedeemDeferredTransferMessage = message<
                uint64_field<"deferred_transfer_transaction_nr", 1>,
                message_field<"transfer", 2, GradidoTransferMessage>
            >;

            using GradidoTimeoutDeferredTransferMessage = message<
                uint64_field<"deferred_transfer_transaction_nr", 1>
            >;

            // register_address.proto
            using RegisterAddressMessage = message <
                bytes_field<"user_pubkey", 1>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                enum_field <"address_type", 2, data::AddressType>,
                bytes_field<"name_hash", 3>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                bytes_field<"account_pubkey", 4>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                uint32_field<"derivation_index", 5>
            >;

            // transaction_body.proto
            using TransactionBodyMessage = message<
                message_field<"memos", 1, EncryptedMemoMessage, repeated>,// singular, std::vector<StringCachedAlloc>>,
                message_field<"created_at", 2, TimestampMessage>,
                string_field<"version_number", 3>,// singular, std::vector<StringCachedAlloc>>,
                enum_field<"type", 4, data::CrossGroupType>,
                string_field<"other_group", 5>,// singular, std::vector<StringCachedAlloc>>,

                // oneof isn't supported from protopuf
                // oneof data
                message_field<"transfer", 6, GradidoTransferMessage>,
                message_field<"creation", 7, GradidoCreationMessage>,
                message_field<"community_friends_update", 8, CommunityFriendsUpdateMessage>,
                message_field<"register_address", 9, RegisterAddressMessage>,
                message_field<"deferred_transfer", 10, GradidoDeferredTransferMessage>,
                message_field<"community_root", 11, CommunityRootMessage>,
                message_field<"redeem_deferred_transfer", 12, GradidoRedeemDeferredTransferMessage>,
                message_field<"timeout_deferred_transfer", 13, GradidoTimeoutDeferredTransferMessage>
            >;

            // ledger_metadata.proto
            using LedgerAnchorMessage = message<
                enum_field<"type", 1, data::LedgerAnchor::Type>,
                // oneof isn't supported from protopuf
                // oneof anchor_id
                bytes_field<"iota_message_id", 2>,
                message_field<"hiero_transaction_id", 3, HieroTransactionIdMessage>,
                uint64_field<"legacy_transaction_id", 4>,
                uint64_field<"node_trigger_transaction_id", 5>
            >;

            // gradido_transaction.proto
            using GradidoTransactionMessage = message<
                message_field<"sig_map", 1, SignatureMapMessage>,
                bytes_field<"body_bytes", 2>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                message_field<"pairing_ledger_anchor", 3, LedgerAnchorMessage>//, singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>
            >;

            // confirmed_transaction.proto
            using ConfirmedTransactionMessage = message<
                uint64_field<"id", 1>,
                message_field<"transaction", 2, GradidoTransactionMessage>,
                message_field<"confirmed_at", 3, TimestampMessage>,
                string_field<"version_number", 4>,// singular, std::vector<StringCachedAlloc>>,
                bytes_field<"running_hash", 5>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                message_field<"ledger_anchor", 6, LedgerAnchorMessage>,// singular, std::vector<ByteVectorCachedAlloc, VectorCacheAllocator<ByteVectorCachedAlloc>>>,
                message_field<"account_balances", 7, AccountBalanceMessage, repeated>,//, singular, std::vector<StringCachedAlloc>>
                enum_field<"balance_derivation", 8, data::BalanceDerivationType>
            >;

            // -- helper classes
            using TransactionTriggerEventMessage = message<
                uint64_field<"linked_transaction_nr", 1>,
                message_field<"target_date", 2, TimestampMessage>,
                enum_field<"type", 3, data::TransactionTriggerEventType>
            >;
        }
    }
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_PROTOPUF_H