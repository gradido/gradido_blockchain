#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONTEXT_H

#include "AbstractRole.h"
#include "TransactionBodyRole.h"
#include "GradidoTransactionRole.h"
#include "ConfirmedTransactionRole.h"
#include "SignatureMapRole.h"

#include <memory>

namespace gradido {
	namespace interaction {
		namespace serialize {
			class GRADIDOBLOCKCHAIN_EXPORT Context
			{
			public:
				Context() = delete;
				Context(const data::TransactionBody& body)
					: mRole(std::make_unique<TransactionBodyRole>(body)) {}
				Context(const data::GradidoTransaction& transaction)
					: mRole(std::make_unique<GradidoTransactionRole>(transaction)) {}
				Context(const data::ConfirmedTransaction& confirmed)
					: mRole(std::make_unique<ConfirmedTransactionRole>(confirmed)) {}
				Context(const data::SignatureMap& signatureMap)
					: mRole(std::make_unique<SignatureMapRole>(signatureMap)) {}
				~Context() {}
				inline memory::ConstBlockPtr run() { return mRole->run(); }

			protected:
				std::unique_ptr<AbstractRole> mRole;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONTEXT_H