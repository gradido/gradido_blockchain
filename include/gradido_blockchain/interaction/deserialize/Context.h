#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONTEXT_H

#include "Type.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/data/TransactionTriggerEvent.h"
#include "gradido_blockchain/data/hiero/AccountId.h"
#include "gradido_blockchain/data/hiero/TopicId.h"
#include "gradido_blockchain/data/hiero/TransactionId.h"

// TODO: rewrite maybe with template traits, example from chatgpt
/*
* template<typename T>
    struct FromTraits; // Primary template (leer)

    // Spezialisierungen
    template<>
    struct FromTraits<data::GradidoTransaction> {
        static std::shared_ptr<const data::GradidoTransaction> run(const memory::ConstBlockPtr& block) {
            // parsing logic hier
            return std::make_shared<data::GradidoTransaction>();
        }
    };

    template<>
    struct FromTraits<data::ConfirmedTransaction> {
        static std::shared_ptr<const data::ConfirmedTransaction> run(const memory::ConstBlockPtr& block) {
            return std::make_shared<data::ConfirmedTransaction>();
        }
    };

    // Generische from<T>
    template<typename T>
    std::shared_ptr<const T> from(const memory::ConstBlockPtr& block) {
        return FromTraits<T>::run(block);
    }
*/

namespace hiero {
	class TransactionId;
}

namespace gradido {
	namespace data {
		class GradidoTransaction;
		class ConfirmedTransaction;
		class TransactionBody;
		class TransactionTriggerEvent;
	}
	namespace interaction {
		namespace deserialize {
			class GRADIDOBLOCKCHAIN_EXPORT Context
			{
			public:
				Context() = delete;
				Context(memory::ConstBlockPtr rawData, Type hint = Type::UNKNOWN)
					: mData(rawData), mType(hint) {}
				~Context();
				void run();

				inline bool isGradidoTransaction() const { return Type::GRADIDO_TRANSACTION == mType; }
				inline bool isTransactionBody() const { return Type::TRANSACTION_BODY == mType; }
				inline bool isConfirmedTransaction() const { return Type::CONFIRMED_TRANSACTION == mType; }
				inline bool isTransactionTriggerEvent() const { return Type::TRANSACTION_TRIGGER_EVENT == mType; }
				inline bool isHieroAccountId() const { return Type::HIERO_ACCOUNT_ID == mType; }
				inline bool isHieroTopicId() const { return Type::HIERO_TOPIC_ID == mType; }
				inline bool isHieroTransactionId() const { return Type::HIERO_TRANSACTION_ID == mType; }
				
				Type getType() const { return mType; }

				inline std::shared_ptr<const data::TransactionBody> getTransactionBody() { return mTransactionBody; }
				inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() { return mGradidoTransaction; }
				inline std::shared_ptr<const data::ConfirmedTransaction> getConfirmedTransaction() { return mConfirmedTransaction; }
				inline hiero::AccountId getHieroAccountId() { return mHieroAccountId; }
				inline hiero::TransactionId getHieroTransactionId() { return mHieroTransactionId; }
				inline hiero::TopicId getHieroTopicId() { return mHieroTopicId; }
				inline data::TransactionTriggerEvent getTransactionTriggerEvent() { return mTransactionTriggerEvent; }

			protected:
				memory::ConstBlockPtr mData;
				Type mType;
				std::shared_ptr<const data::TransactionBody> mTransactionBody;
				std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
				std::shared_ptr<const data::ConfirmedTransaction> mConfirmedTransaction;
				hiero::AccountId mHieroAccountId;
				hiero::TopicId mHieroTopicId;
				hiero::TransactionId mHieroTransactionId;
				data::TransactionTriggerEvent mTransactionTriggerEvent;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONTEXT_H