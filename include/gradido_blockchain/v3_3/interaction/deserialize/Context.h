#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_DESERIALIZE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_DESERIALIZE_CONTEXT_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "TransactionBodyRole.h"
#include "GradidoTransactionRole.h"
#include "ConfirmedTransactionRole.h"
#include "Type.h"

namespace gradido {
	namespace v3_3 {
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

					inline data::ConstTransactionBodyPtr getTransactionBody() { return mTransactionBody; }
					inline std::shared_ptr<const data::GradidoTransaction> getGradidoTransaction() { return mGradidoTransaction; }
					inline std::shared_ptr<const data::ConfirmedTransaction> getConfirmedTransaction() { return mConfirmedTransaction; }

				protected:
					memory::ConstBlockPtr mData;
					Type mType;
					data::ConstTransactionBodyPtr mTransactionBody;
					std::shared_ptr<const data::GradidoTransaction> mGradidoTransaction;
					std::shared_ptr<const data::ConfirmedTransaction> mConfirmedTransaction;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_CONTEXT_H