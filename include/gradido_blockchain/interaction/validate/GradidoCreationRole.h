#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H

#include "AbstractRole.h"
#include "Error.h"
#include "Options.h"

namespace gradido {
	class AppContext;
	namespace data {
		class GradidoCreation;
		namespace compact {
			struct ConfirmedGradidoTx;
		}
	}

	namespace interaction::validate {

		GRADIDOBLOCKCHAIN_EXPORT Error validateGradidoCreation(const data::compact::ConfirmedGradidoTx& tx, const AppContext& appContext, Options options);

		class GradidoCreationRole : public AbstractRole
		{
		public:
			GradidoCreationRole(std::shared_ptr<const data::GradidoCreation> gradidoCreation);

			void validateTargetDate(Timepoint receivedTimePoint);
			void run(Type type, ContextData& c);

			void checkRequiredSignatures(
				const data::SignatureMap& signatureMap,
				std::shared_ptr<blockchain::Abstract> blockchain = nullptr
			) const;
		protected:

			std::shared_ptr<const data::GradidoCreation> mGradidoCreation;
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_GRADIDO_CREATION_ROLE_H