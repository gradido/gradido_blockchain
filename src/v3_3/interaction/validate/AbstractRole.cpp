#include "gradido_blockchain/v3_3/interaction/validate/AbstractRole.h"

#include <regex>

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
			
				std::regex g_RegExCommunityAlias("^[a-z0-9-]{3,120}$");

				bool AbstractRole::isValidCommunityAlias(const std::string& communityAlias)
				{
					return std::regex_match(communityAlias, g_RegExCommunityAlias);
				}

			}
		}
	}
}