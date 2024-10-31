#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_FILTER_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_FILTER_ROLE_H

#include "AbstractRole.h"

namespace gradido {
	namespace blockchain {
		class Filter;
	}
	namespace interaction {
		namespace toJson {
			class FilterRole : public AbstractRole
			{
			public:
				FilterRole(const blockchain::Filter& filter)
					: mFilter(filter) {}

				rapidjson::Value composeJson(rapidjson::Document& rootDocument) const;
        
			protected:
				const blockchain::Filter& mFilter;
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_TO_JSON_FILTER_ROLE_H