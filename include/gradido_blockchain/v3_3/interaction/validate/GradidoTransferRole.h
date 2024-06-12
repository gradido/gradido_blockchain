#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H

#include "AbstractRole.h"
#include "gradido_blockchain/v3_3/data/Protocol.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
				class GradidoTransferRole : public AbstractRole
				{
				public:
					GradidoTransferRole(const data::GradidoTransfer& gradidoTransfer)
						: mGradidoTransfer(gradidoTransfer) {}

					void run(Type type = Type::SINGLE, const std::string& communityId, std::shared_ptr<AbstractBlockchainProvider> blockchainProvider);
				protected:

					const data::GradidoTransfer& mGradidoTransfer;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_GRADIDO_TRANSFER_ROLE_H