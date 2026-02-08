#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ZIG_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ZIG_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class ConfirmedTransaction;
	}
	namespace interaction {
		namespace serialize {

			class ConfirmedTransactionZigRole : public AbstractRole
			{
			public:
				ConfirmedTransactionZigRole(const data::ConfirmedTransaction& confirmedTransaction);
				~ConfirmedTransactionZigRole();

				memory::ConstBlockPtr run() const override;
				size_t calculateSerializedSize() const override;

			protected:
				const data::ConfirmedTransaction& mConfirmedTransaction;
			};

		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_CONFIRMED_TRANSACTION_ZIG_ROLE_H