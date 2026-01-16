#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_DEFERRED_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_DEFERRED_TRANSFER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace data {
		class GradidoDeferredTransfer;
	}
	namespace interaction {
		namespace validate {
			class GradidoDeferredTransferRole : public AbstractRole
			{
			public:
				GradidoDeferredTransferRole(std::shared_ptr<const data::GradidoDeferredTransfer> deferredTransfer);

				void run(Type type, ContextData& c);
			protected:
				std::shared_ptr<const data::GradidoDeferredTransfer> mDeferredTransfer;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_DEFERRED_TRANSFER_ROLE_H