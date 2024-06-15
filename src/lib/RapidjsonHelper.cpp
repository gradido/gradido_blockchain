#include "gradido_blockchain/lib/RapidjsonHelper.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "magic_enum/magic_enum.hpp"

using namespace magic_enum;

using namespace rapidjson;

namespace rapidjson_helper
{
	const char* getMemberTypeString(MemberType type)
	{
		switch (type) {
		case MemberType::STRING: return "string";
		case MemberType::INTEGER: return "integer";
		case MemberType::FLOAT: return "float";
		case MemberType::NUMBER: return "number";
		case MemberType::DATETIME: return "date time";
		case MemberType::ARRAY: return "array";
		default: throw GradidoUnknownEnumException("unknown enum value in model::graphql::Base", "MemberType", enum_name(type).data());
		}
	}


	void checkMember(const Value& obj, const char* memberName, MemberType type, const char* objectName /* = nullptr*/)
	{
		if (!obj.HasMember(memberName)) {
			std::string what = "missing member in ";
			if (objectName) {
				what += objectName;
			}
			throw RapidjsonMissingMemberException(what.data(), memberName, getMemberTypeString(type));
		}

		std::string what = "invalid member in ";
		if (objectName) {
			what += objectName;
		}

		bool wrongType = false;

		// int timezoneDifferential = Poco::Timezone::tzd();

		switch (type) {
		case MemberType::STRING:
			if (!obj[memberName].IsString()) wrongType = true;
			break;
		case MemberType::DATETIME:
			if (!obj[memberName].IsString()) wrongType = true;
			break;
		case MemberType::INTEGER:
			if (!obj[memberName].IsInt()) wrongType = true;
			break;
		case MemberType::FLOAT:
			if (!obj[memberName].IsFloat()) wrongType = true;
			break;
		case MemberType::NUMBER:
			if (!obj[memberName].IsNumber()) wrongType = true;
			break;
		case MemberType::ARRAY:
			if (!obj[memberName].IsArray()) wrongType = true;
			break;
		default: throw GradidoUnknownEnumException("unknown enum value in model::graphql::Base", "MemberType", enum_name(type).data());
		}

		if (wrongType) {
			throw RapidjsonInvalidMemberException(what.data(), memberName, getMemberTypeString(type));
		}
	}
}