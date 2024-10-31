#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONTEXT_H

#include "Type.h"
#include "gradido_blockchain/memory/Block.h"

namespace gradido {
	namespace data {
		class GradidoTransaction;
		class ConfirmedTransaction;
		class TransactionBody;
	}
	namespace interaction {
		namespace deserialize {
			class GRADIDOBLOCKCHAIN_EXPORT Context
			{
			public:
				Context() = delete;
				Context(memory::ConstBlockPtr rawData, Type hint = Type::UNKNOWN)
					: mData(rawData), mType(hint) {}
				~Context() {}
				void run();

				inline bool isGradidoTransaction() const { return Type::GRADIDO_TRANSACTION == mType; }
				inline bool isTransactionBody() const { return Type::TRANSACTION_BODY == mType; }
				inline bool isConfirmedTransaction() const { return Type::CONFIRMED_TRANSACTION == mType; }
				Type getType() const { return mType; }

				inline std::shared_ptr<const data::TransactionBody> getTransactionBody() { return mTransactionBody; }
				inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() { return mGradidoTransaction; }
				inline std::shared_ptr<const data::ConfirmedTransaction> getConfirmedTransaction() { return mConfirmedTransaction; }

			protected:
				memory::ConstBlockPtr mData;
				Type mType;
				std::shared_ptr<const data::TransactionBody> mTransactionBody;
				std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
				std::shared_ptr<const data::ConfirmedTransaction> mConfirmedTransaction;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_CONTEXT_H