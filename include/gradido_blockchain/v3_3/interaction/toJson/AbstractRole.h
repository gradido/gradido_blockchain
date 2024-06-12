#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_TO_JSON_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_TO_JSON_ABSTRACT_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "AbstractRole.h"
#include "rapidjson/document.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace toJson {
				class AbstractRole
				{
				public:
					// return json string
					virtual const char* run(bool pretty) const = 0;
				protected:
					const char* toString(const rapidjson::Document* document, bool pretty = false) const;
				};
			}
		}
	}
}

#endif __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_TO_JSON_ABSTRACT_ROLE_H