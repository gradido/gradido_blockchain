#include "gradido_blockchain/interaction/toJson/AbstractRole.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

namespace gradido {
	namespace interaction {
		namespace toJson {
			const char* AbstractRole::toString(const rapidjson::Document* document, bool pretty/* = false*/) const
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
