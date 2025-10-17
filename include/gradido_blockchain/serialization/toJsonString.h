#ifndef __GRADIDO_BLOCKCHAIN_SERIALIZATION_TO_JSON_STRING_H__
#define __GRADIDO_BLOCKCHAIN_SERIALIZATION_TO_JSON_STRING_H__

#include "toJson.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include <string>

namespace serialization {
  template<typename T>
  std::string toJsonString(const T& value, bool pretty = false) {
    rapidjson::Document doc(rapidjson::kObjectType);
    auto& alloc = doc.GetAllocator();
    auto jsonValue = toJson(value, alloc);
    rapidjson::StringBuffer buffer;
    if (pretty) {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
      jsonValue.Accept(writer);
    } else {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      jsonValue.Accept(writer);
    }
    return buffer.GetString();
  }
}
#endif // __GRADIDO_BLOCKCHAIN_SERIALIZATION_TO_JSON_STRING_H__
