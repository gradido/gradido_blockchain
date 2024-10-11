#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_ABSTRACT_ROLE_H

#include "rapidjson/document.h"
#include <string>

namespace gradido {
	namespace interaction {
		namespace toJson {
			class AbstractRole
			{
			public:
				//! return json string
				std::string run(bool pretty) const;
				virtual rapidjson::Value composeJson(rapidjson::Document& rootDocument) const = 0;
			protected:
				std::string toString(const rapidjson::Document* document, bool pretty = false) const;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_ABSTRACT_ROLE_H