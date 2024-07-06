#include "gradido_blockchain/interaction/toJson/AbstractRole.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace gradido {
	namespace interaction {
		namespace toJson {

			std::string AbstractRole::run(bool pretty) const
			{
				Document root(kObjectType);
				auto value = composeJson(root);
				root.Swap(value);
				return toString(&root, pretty);
			}
			std::string AbstractRole::toString(const rapidjson::Document* document, bool pretty/* = false*/) const
			{
				StringBuffer buffer;
				if (pretty) {
					PrettyWriter<StringBuffer> writer(buffer);
					document->Accept(writer);
				}
				else {
					Writer<StringBuffer> writer(buffer);
					document->Accept(writer);
				}
				return buffer.GetString();
			}
		}
	}
}
