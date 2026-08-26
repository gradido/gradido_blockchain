#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class ConfirmedTransaction;
	}
	namespace interaction {
		namespace serialize {

			class ConfirmedTransactionRole : public AbstractRole
			{
			public:
				ConfirmedTransactionRole(const data::ConfirmedTransaction& confirmedTransaction);
				~ConfirmedTransactionRole();

				memory::ConstBlockPtr run() const override;
				size_t calculateSerializedSize() const override;

			protected:
				const data::ConfirmedTransaction& mConfirmedTransaction;
			};

		}
	}
}

#endif //GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ROLE_H