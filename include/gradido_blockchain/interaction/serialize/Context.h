#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONTEXT_H

#include "AbstractRole.h"
#include "ConfirmedTransactionRole.h"
#include "GradidoTransactionRole.h"
#include "HieroAccountIdRole.h"
#include "HieroTopicIdRole.h"
#include "HieroTransactionIdRole.h"
#include "LedgerAnchorRole.h"
#include "SignatureMapRole.h"
#include "TransactionBodyRole.h"
#include "TransactionTriggerEventRole.h"

#include <memory>

namespace gradido {
	namespace interaction {
		namespace serialize {

			class GRADIDOBLOCKCHAIN_EXPORT Context
			{
			public:
				Context() = delete;
				Context(const data::ConfirmedTransaction& confirmed)
					: mRole(std::make_unique<ConfirmedTransactionRole>(confirmed)) {}
				Context(const data::GradidoTransaction& transaction)
					: mRole(std::make_unique<GradidoTransactionRole>(transaction)) {}
				Context(const hiero::AccountId& accountId)
					: mRole(std::make_unique<HieroAccountIdRole>(accountId)) {}
				Context(const hiero::TopicId& topicId)
					: mRole(std::make_unique<HieroTopicIdRole>(topicId)) {}
				Context(const hiero::TransactionId& transactionId)
					: mRole(std::make_unique<HieroTransactionIdRole>(transactionId)) {}
				Context(const data::LedgerAnchor& ledgerAnchor)
					: mRole(std::make_unique<LedgerAnchorRole>(ledgerAnchor)) {}
				Context(const data::SignatureMap& signatureMap)
					: mRole(std::make_unique<SignatureMapRole>(signatureMap)) {}
				Context(const data::TransactionBody& body)
					: mRole(std::make_unique<TransactionBodyRole>(body)) {}
				Context(const data::TransactionTriggerEvent& transactionTriggerEvent)
					: mRole(std::make_unique<TransactionTriggerEventRole>(transactionTriggerEvent)) {}
				
				// move constructor & assignment
				Context(Context&&) noexcept = default;
				Context& operator=(Context&&) noexcept = default;

				// delete copy constructor & assignment
				Context(const Context&) = delete;
				Context& operator=(const Context&) = delete;

				~Context() {}
				inline memory::ConstBlockPtr run() { return mRole->run(); }

			protected:
				std::unique_ptr<AbstractRole> mRole;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONTEXT_H