#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_CONTEXT_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "AbstractRole.h"
#include "TransactionBodyRole.h"
#include "Type.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {
				class GRADIDOBLOCKCHAIN_EXPORT Context
				{
				public:
					Context() = delete;
					Context(const data::TransactionBody& body)
						: mType(Type::TRANSACTION_BODY), mRole(std::make_unique<TransactionBodyRole>(body)) {}
					// Context(const data::GradidoTransaction& transaction);
					// Context(const data::ConfirmedTransaction& confirmed);
					~Context() {}
					inline memory::ConstBlockPtr run() { return mRole->run(); }

				protected:
					
					Type mType;
					std::unique_ptr<AbstractRole> mRole;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_CONTEXT_H