#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_ABSTRACT_ROLE_H

#include <memory>

namespace gradido {
	namespace blockchain {
		class Abstract;
	}
	namespace data {
		class GradidoTransaction;
	}
	namespace interaction {
		namespace createTransactionByEvent {
			class AbstractRole
			{
			public:
				virtual ~AbstractRole() {}
				virtual std::unique_ptr<data::GradidoTransaction> run(const blockchain::Abstract& blockchain) const = 0;
			protected:
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_CREATE_TRANSACTION_BY_EVENT_ABSTRACT_ROLE_H