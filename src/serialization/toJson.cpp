#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/crypto/ByteArray.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/serialization/toJson.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/types.h"

#include "sodium.h"

#include "date/date.h"
#include <chrono>
#include <iomanip>
#include <optional>
#include <sstream>

using namespace rapidjson;

namespace serialization {

	template<>
	Value toJson(const memory::Block& block, Document::AllocatorType& alloc) 
	{
		if (!block.size()) {
			return Value("");
		}
		uint32_t hexSize = block.size() * 2 + 1;
		memory::Block hexMem(hexSize);
		sodium_bin2hex((char*)hexMem.data(), hexSize, block, block.size());
		return Value((char*)hexMem.data(), hexMem.size() - 1, alloc);
	}

	template<>
	Value toJson(const memory::ConstBlockPtr& ptr, Document::AllocatorType& alloc)
	{
		if (!ptr) {
			return Value("");
		}
		return toJson(*ptr, alloc);
	}

	template<>
	Value toJson(const Timepoint& timepoint, Document::AllocatorType& alloc) 
	{
		std::string utcTimepointString = DataTypeConverter::timePointToString(timepoint) + "Z";
		return toJson(utcTimepointString, alloc);
	}

	template<>
	Value toJson(const Duration& duration, Document::AllocatorType& alloc)
	{
		return toJson(DataTypeConverter::timespanToString(duration), alloc);
	}

	template<>
	Value toJson(const date::year& year, Document::AllocatorType& alloc)
	{
		return toJson(std::to_string(static_cast<int>(year)), alloc);
	}
	
	template<>
	Value toJson(const date::month& month, Document::AllocatorType& alloc)
	{
		return toJson(std::to_string(static_cast<unsigned int>(month)), alloc);
	}
	
	template<>
	Value toJson(const GradidoUnit& gdd, Document::AllocatorType& alloc)
	{
		return toJson(gdd.toString(4), alloc);
	}

#ifdef _WIN32
	// Explicitly instantiate and export these specializations from the library.
	// This is required on Windows when building a dynamic/shared library to ensure
	// the symbols are visible to consuming code.
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const memory::Block& block, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const memory::ConstBlockPtr& ptr, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const Timepoint& timepoint, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const Duration& duration, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const date::year& year, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const date::month& month, Document::AllocatorType& alloc);
	template GRADIDOBLOCKCHAIN_EXPORT Value toJson(const GradidoUnit& gdd, Document::AllocatorType& alloc);
#endif
}