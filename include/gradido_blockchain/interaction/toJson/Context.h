#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONTEXT_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONTEXT_H

#include "TransactionBodyRole.h"
#include "GradidoTransactionRole.h"
#include "ConfirmedTransactionRole.h"
#include "FilterRole.h"

namespace gradido {
	namespace interaction {
		namespace toJson {
			class GRADIDOBLOCKCHAIN_EXPORT Context 
			{
			public:
				Context(const data::TransactionBody& body)
					: mRole(std::make_unique<TransactionBodyRole>(body)) {}
				Context(const data::GradidoTransaction& gradidoTransaction, BodyBytesType format = BodyBytesType::JSON)
					: mRole(std::make_unique<GradidoTransactionRole>(gradidoTransaction, format)) {}
				Context(const data::ConfirmedTransaction& confirmedTransaction, BodyBytesType format = BodyBytesType::JSON)
					: mRole(std::make_unique<ConfirmedTransactionRole>(confirmedTransaction, format)) {}
				Context(const blockchain::Filter& filter)
					: mRole(std::make_unique<FilterRole>(filter)) {}

				inline std::string run(bool pretty = false) const { return mRole->run(pretty);}
				//! use this if you like to add this json to you own json tree
				inline rapidjson::Value run(rapidjson::Document& rootJson) const { return mRole->composeJson(rootJson); }
			protected:
				std::unique_ptr<AbstractRole> mRole;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_CONTEXT_H