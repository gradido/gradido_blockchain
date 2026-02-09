#ifndef __GRADIDO_BLOCKCHAIN_SERIALIZATION_TO_JSON_H__
#define __GRADIDO_BLOCKCHAIN_SERIALIZATION_TO_JSON_H__

#include <rapidjson/document.h>
#include <string>
#include <vector>
#include <map>
#include <type_traits>

#include "magic_enum/magic_enum.hpp"

namespace gradido {
	namespace blockchain {
		class Filter;
	}
}

namespace serialization {

// Explicitly instantiate and export these specializations from the library.
// This is required on Windows when building a dynamic/shared library to ensure
// the symbols are visible to consuming code.
#ifdef _WIN32
#define DEFINE_TO_JSON(TYPE, BODY)												 \
    template<>																	 \
    rapidjson::Value toJson(const TYPE& value, Document::AllocatorType& alloc) { \
        rapidjson::Value obj(rapidjson::kObjectType);                            \
        BODY																	 \
        return obj;																 \
    }																			 \
    template GRADIDOBLOCKCHAIN_EXPORT rapidjson::Value toJson(const TYPE& value, Document::AllocatorType& alloc);
// DEFINE_TO_JSON end
#else
#define DEFINE_TO_JSON(TYPE, BODY)												 \
    template<>																	 \
    rapidjson::Value toJson(const TYPE& value, Document::AllocatorType& alloc) { \
        rapidjson::Value obj(rapidjson::kObjectType);                            \
        BODY																	 \
        return obj;																 \
    }
// DEFINE_TO_JSON end
#endif

	// for compile time check if for a type a toJson specialization exists
	// C++20 concept-based detection - works reliably with Clang (zig c++)
	template<typename T>
	concept HasToJson = requires(const T& t, rapidjson::Document::AllocatorType& alloc) {
			{ toJson(t, alloc) } -> std::same_as<rapidjson::Value>;
	};

	// Generic converter
	template<typename T>
	rapidjson::Value toJson(const T& value, rapidjson::Document::AllocatorType& alloc);

	// Specialization for containers
	// Default: no container
	template<typename T>
	struct is_json_container : std::false_type {};

	// SFINAE: all types with const begin()/end() are containers
	template<typename T>
	requires requires(const T t) { t.begin(); t.end(); }
	struct is_json_container<T> : std::true_type {};

	// Explicitly exclude
	template<>
	struct is_json_container<class TimepointInterval> : std::false_type {};

	// Helper Variable
	template<typename T>
	inline constexpr bool is_json_container_v = is_json_container<T>::value;

	template<typename T>
	requires (!HasToJson<T>) && is_json_container_v<T>
	rapidjson::Value toJson(const T& container, rapidjson::Document::AllocatorType& alloc)
	{
		rapidjson::Value arr(rapidjson::kArrayType);
		for (const auto& el : container) {
			arr.PushBack(toJson(el, alloc), alloc);
		}
		return arr;
	}

	// Specialization for maps
	template<typename K, typename V>
	rapidjson::Value toJson(const std::map<K, V>& m, rapidjson::Document::AllocatorType& alloc)
	{
		rapidjson::Value obj(rapidjson::kObjectType);
		for (auto& [k,v] : m)
		{
			obj.AddMember(
				rapidjson::Value(std::to_string(k).c_str(), alloc),
				toJson(v, alloc), alloc
			);
		}
		return obj;
	}

	// Fallback for types with their own JsonConverter
	/*template<typename T>
	requires has_to_json_v<T>
	rapidjson::Value toJson(const T& value, rapidjson::Document::AllocatorType& alloc)
	{
		return toJson<T>(value, alloc);
	}*/

	// only for static char* which live as long as the program
	template <size_t N>
	inline rapidjson::Value toJson(const char (&s)[N], rapidjson::Document::AllocatorType& alloc) {
		static_assert(N > 0, "String must not be empty");
		return rapidjson::Value(rapidjson::StringRef(s));
	}

	template<>
	inline rapidjson::Value toJson(const std::string& s, rapidjson::Document::AllocatorType& alloc) {
		return rapidjson::Value(s.data(), s.size(), alloc);
	}

	template<typename T>
	requires std::is_enum_v<T>
	rapidjson::Value toJson(const T& value, rapidjson::Document::AllocatorType& alloc) {
		return toJson(std::string(magic_enum::enum_name(value)), alloc);
	}
}

#endif // __GRADIDO_BLOCKCHAIN_SERIALIZATION_TO_JSON_H__