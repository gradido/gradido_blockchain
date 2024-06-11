#include "gradido_blockchain/v3_3/interaction/validate/Context.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {

				void Context::run(const data::TransactionBody& body, Type type/* = Type::SINGLE*/, const std::string& communityId/* = ""*/)
				{

				}

				void Context::run(const data::ConfirmedTransaction& confirmedTransaction, Type type/* = Type::SINGLE*/, const std::string& communityId/* = ""*/)
				{

				}
				void Context::run(const data::GradidoTransaction& gradidoTransaction, Type type/* = Type::SINGLE*/, const std::string& communityId/* = ""*/)
				{

				}

			}
		}
	}
}