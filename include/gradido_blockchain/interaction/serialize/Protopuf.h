#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_PROTOPUF_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_PROTOPUF_H

#include <string>
#include <bit>
#include "protopuf/message.h"

#include "gradido_blockchain/data/TransactionTriggerEventType.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"
#include "gradido_blockchain_core/types/ledger_anchor.h"

using namespace pp;
using namespace memory;
using namespace std;

namespace gradido {
    namespace interaction {
        namespace serialize {

            //  ----------------  basic_types.proto -----------------------------------
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

            //  ---------------- end   basic_types.proto   end -----------------------------------
            // 

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

            // ledger_metadata.proto
            using LedgerAnchorMessage = message<
                enum_field<"type", 1, grdt_ledger_anchor>,
                // oneof isn't supported from protopuf
                // oneof anchor_id
                // bytes_field<"iota_message_id", 2>,
                message_field<"hiero_transaction_id", 3, HieroTransactionIdMessage>,
                uint64_field<"id", 4>
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

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_PROTOPUF_H