#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_SIGNATURE_MAP_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_SIGNATURE_MAP_ROLE_H

#include "AbstractRole.h"
#include "Protopuf.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {
				class SignatureMapRole : public AbstractRole
				{
				public:
					SignatureMapRole(const data::SignatureMap& signatureMap)
						: mSignatureMap(signatureMap) {}
					~SignatureMapRole() {};

					RUM_IMPLEMENTATION
					SignatureMapMessage getMessage() const;
					size_t calculateSerializedSize() const;
				protected:
					const data::SignatureMap& mSignatureMap;
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_SIGNATURE_MAP_ROLE_H