#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "sodium.h"

#include <stdexcept>
#include <assert.h>
#include <regex>
#include <iomanip>

// needed for memset in linux
#include <string.h>

using namespace std::chrono;

namespace DataTypeConverter
{
	std::regex g_rexExpBase64("^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{2}==|[A-Za-z0-9+/]{3}=)?$");

	NumberParseState strToInt(const std::string& input, int& result)
	{
		try {
			result = stoi(input);
			return NUMBER_PARSE_OKAY;
		}
		catch (const std::invalid_argument& ia)
		{
			printf("[strToInt] exception: invalid argument: %s\n", ia.what());
			return NUMBER_PARSE_INVALID_ARGUMENT;
		}
		catch (const std::out_of_range& oor)
		{
			printf("[strToInt] exception: out or range: %s\n", oor.what());
			return NUMBER_PARSE_OUT_OF_RANGE;
		}
		catch (const std::logic_error & ler)
		{
			printf("[strToInt] exception: logical error: %s\n", ler.what());
			return NUMBER_PARSE_LOGIC_ERROR;
		}
	}
#ifdef __linux__
	NumberParseState strToInt(const std::string& input, unsigned long long& result)
	{
		try {
			result = stoull(input);
			return NUMBER_PARSE_OKAY;
		}
		catch (const std::invalid_argument& ia)
		{
			printf("[strToInt] exception: invalid argument: %s\n", ia.what());
			return NUMBER_PARSE_INVALID_ARGUMENT;
		}
		catch (const std::out_of_range& oor)
		{
			printf("[strToInt] exception: out or range: %s\n", oor.what());
			return NUMBER_PARSE_OUT_OF_RANGE;
		}
		catch (const std::logic_error & ler)
		{
			printf("[strToInt] exception: logical error: %s\n", ler.what());
			return NUMBER_PARSE_LOGIC_ERROR;
		}
	}
#endif
	NumberParseState strToInt(const std::string& input, Poco::UInt64& result)
	{
		try {
			result = stoull(input);
			return NUMBER_PARSE_OKAY;
		}
		catch (const std::invalid_argument& ia)
		{
			printf("[strToInt] exception: invalid argument: %s\n", ia.what());
			return NUMBER_PARSE_INVALID_ARGUMENT;
		}
		catch (const std::out_of_range& oor)
		{
			printf("[strToInt] exception: out or range: %s\n", oor.what());
			return NUMBER_PARSE_OUT_OF_RANGE;
		}
		catch (const std::logic_error & ler)
		{
			printf("[strToInt] exception: logical error: %s\n", ler.what());
			return NUMBER_PARSE_LOGIC_ERROR;
		}
	}

	uint64_t strToInt(const std::string& input)
	{
		try {
			return stoull(input);
		}
		catch (const std::invalid_argument& ia)
		{
			throw ParseNumberException(ia.what(), NUMBER_PARSE_INVALID_ARGUMENT);
		}
		catch (const std::out_of_range& oor)
		{
			throw ParseNumberException(oor.what(), NUMBER_PARSE_OUT_OF_RANGE);
		}
		catch (const std::logic_error& ler)
		{
			throw ParseNumberException(ler.what(), NUMBER_PARSE_LOGIC_ERROR);
		}
	}

	NumberParseState strToDouble(const std::string& input, double& result)
	{
		auto comma_position = input.find(',');
		std::string input_filtered = input;
		if (comma_position > 0 && comma_position != input.npos) {
			input_filtered = input_filtered.replace(comma_position, 0, 1, '.');
		}
		try {
			result = stod(input_filtered);
			return NUMBER_PARSE_OKAY;
		}
		catch (const std::invalid_argument& ia)
		{
			printf("[strToDouble] exception: invalid argument: %s\n", ia.what());
			return NUMBER_PARSE_INVALID_ARGUMENT;
		}
		catch (const std::out_of_range& oor)
		{
			printf("[strToDouble] exception: out or range: %s\n", oor.what());
			return NUMBER_PARSE_OUT_OF_RANGE;
		}
		catch (const std::logic_error & ler)
		{
			printf("[strToDouble] exception: logical error: %s\n", ler.what());
			return NUMBER_PARSE_LOGIC_ERROR;
		}
	}

	const char* numberParseStateToString(NumberParseState state)
	{
		switch (state) {
		case NUMBER_PARSE_OKAY: return "okay";
		case NUMBER_PARSE_INVALID_ARGUMENT: return "invalid argument";
		case NUMBER_PARSE_OUT_OF_RANGE: return "out of range";
		case NUMBER_PARSE_LOGIC_ERROR: return "logical error";
		}
		return "<unknown>";
	}

	MemoryBin* hexToBin(const std::string& hexString)
	{
		/*
		int sodium_hex2bin(unsigned char * const bin, const size_t bin_maxlen,
		const char * const hex, const size_t hex_len,
		const char * const ignore, size_t * const bin_len,
		const char ** const hex_end);

		The sodium_hex2bin() function parses a hexadecimal string hex and converts it to a byte sequence.

		hex does not have to be nul terminated, as the number of characters to parse is supplied via the hex_len parameter.

		ignore is a string of characters to skip. For example, the string ": " allows columns and spaces to be present at any locations in the hexadecimal string. These characters will just be ignored. As a result, "69:FC", "69 FC", "69 : FC" and "69FC" will be valid inputs, and will produce the same output.

		ignore can be set to NULL in order to disallow any non-hexadecimal character.

		bin_maxlen is the maximum number of bytes to put into bin.

		The parser stops when a non-hexadecimal, non-ignored character is found or when bin_maxlen bytes have been written.

		If hex_end is not NULL, it will be set to the address of the first byte after the last valid parsed character.

		The function returns 0 on success.

		It returns -1 if more than bin_maxlen bytes would be required to store the parsed string, or if the string couldn't be fully parsed, but a valid pointer for hex_end was not provided.

		It evaluates in constant time for a given length and format.
		*/

		auto mm = MemoryManager::getInstance();
		size_t hexSize = hexString.size();
		size_t binSize = (hexSize) / 2;
		MemoryBin* bin = mm->getMemory(binSize);
		memset(*bin, 0, binSize);

		size_t resultBinSize = 0;

		if (0 != sodium_hex2bin(*bin, binSize, hexString.data(), hexSize, nullptr, &resultBinSize, nullptr)) {
			mm->releaseMemory(bin);
			// TODO: throw InvalidHexException
			return nullptr;
		}
		return bin;

	}

	MemoryBin* hexToBin(const char* hexString, size_t stringSize)
	{
		auto mm = MemoryManager::getInstance();
		size_t binSize = (stringSize) / 2;
		MemoryBin* bin = mm->getMemory(binSize);
		memset(*bin, 0, binSize);

		size_t resultBinSize = 0;

		if (0 != sodium_hex2bin(*bin, binSize, hexString, stringSize, nullptr, &resultBinSize, nullptr)) {
			mm->releaseMemory(bin);
			// TODO: throw InvalidHexException
			return nullptr;
		}
		return bin;
	}

	std::unique_ptr<std::string> hexToBinString(const std::string& hexString)
	{
		assert(hexString.size());
		if (hexString.size() % 2 != 0) {
			throw InvalidHexException("invalid hex, size not divisible by two");
		}
		auto mm = MemoryManager::getInstance();
		size_t hexSize = hexString.size();
		size_t binSize = (hexSize) / 2;
		std::unique_ptr<std::string> binString(new std::string(binSize, 0));
		
		size_t resultBinSize = 0;

		if (0 != sodium_hex2bin((unsigned char*)binString->data(), binSize, hexString.data(), hexSize, nullptr, &resultBinSize, nullptr)) {
			// TODO: throw InvalidHexException
			throw InvalidHexException("invalid hex");
		}
		return binString;
	}

	MemoryBin* base64ToBin(const std::string& base64String, int variant /*= sodium_base64_VARIANT_ORIGINAL*/)
	{
		/*
		int sodium_base642bin(unsigned char * const bin, const size_t bin_maxlen,
		const char * const b64, const size_t b64_len,
		const char * const ignore, size_t * const bin_len,
		const char ** const b64_end, const int variant);

		sodium_base64_VARIANT_ORIGINAL
		*/
		auto mm = MemoryManager::getInstance();
		size_t encodedSize = base64String.size();
		size_t binSize = (encodedSize / 4) * 3;
		auto bin = mm->getMemory(binSize);
		memset(*bin, 0, binSize);

		size_t resultBinSize = 0;

		auto convertResult = sodium_base642bin(*bin, binSize, base64String.data(), encodedSize, nullptr, &resultBinSize, nullptr, variant);
		if (0 != convertResult) {
			mm->releaseMemory(bin);
			throw GradidoInvalidBase64Exception("invalid base64", base64String.data(), convertResult);
		}
		if (resultBinSize < binSize) {
			auto bin_real = mm->getMemory(resultBinSize);
			memcpy(*bin_real, *bin, resultBinSize);
			mm->releaseMemory(bin);
			return bin_real;
		}

		return bin;
	}

	std::unique_ptr<std::string> base64ToBinString(std::unique_ptr<std::string> base64String, int variant/* = sodium_base64_VARIANT_ORIGINAL*/)
	{
		auto mm = MemoryManager::getInstance();
		size_t encodedSize = base64String->size();
		size_t binSize = (encodedSize / 4) * 3;
		MemoryBin* bin = mm->getMemory(binSize);
		memset(*bin, 0, binSize);

		size_t resultBinSize = 0;
		auto convertResult = sodium_base642bin(*bin, binSize, base64String->data(), encodedSize, nullptr, &resultBinSize, nullptr, variant);
		if (0 != convertResult) {
			mm->releaseMemory(bin);
			throw GradidoInvalidBase64Exception("invalid base64", base64String.release()->data(), convertResult);
		}
		base64String->reserve(resultBinSize);
		base64String->assign((const char*)*bin, resultBinSize);
		mm->releaseMemory(bin);
		return base64String;
	}


	std::string binToBase64(const unsigned char* data, size_t size, int variant /*= sodium_base64_VARIANT_ORIGINAL*/)
	{
		auto mm = MemoryManager::getInstance();

		size_t encodedSize = sodium_base64_encoded_len(size, variant);
		auto base64 = mm->getMemory(encodedSize);
		memset(*base64, 0, encodedSize);

		if (nullptr == sodium_bin2base64(*base64, encodedSize, data, size, variant)) {
			mm->releaseMemory(base64);
			return "";
		}

		std::string base64String((const char*)*base64, encodedSize-1);
		mm->releaseMemory(base64);
		return base64String;
	}

	std::unique_ptr<std::string> binToBase64(std::unique_ptr<std::string> proto_bin, int variant/* = sodium_base64_VARIANT_ORIGINAL*/)
	{
		auto mm = MemoryManager::getInstance();

		// return encodedSize + 1 for trailing \0
		size_t encodedSize = sodium_base64_encoded_len(proto_bin->size(), variant);
		auto base64 = mm->getMemory(encodedSize);
		memset(*base64, 0, encodedSize);

		if (nullptr == sodium_bin2base64(*base64, encodedSize, (const unsigned char*)proto_bin->data(), proto_bin->size(), variant)) {
			mm->releaseMemory(base64);
			return nullptr;
		}
		// we don't need a trailing \0 because string already store the size
		proto_bin->reserve(encodedSize-1);
		proto_bin->assign((const char*)*base64, encodedSize-1);
		
		mm->releaseMemory(base64);
		return proto_bin;
	}
	
	 
	std::string binToHex(const unsigned char* data, size_t size) 
	{
		auto mm = MemoryManager::getInstance();
		size_t hexSize = size * 2 + 1;
		size_t binSize = size;
		MemoryBin* hex = mm->getMemory(hexSize);
		memset(*hex, 0, hexSize);

		size_t resultBinSize = 0;

		sodium_bin2hex(*hex, hexSize, data, binSize);

		std::string hexString((const char*)*hex, hexSize - 1);
		mm->releaseMemory(hex);
		return hexString;
	}

	std::unique_ptr<std::string> binToHex(std::unique_ptr<std::string> binString)
	{
		auto mm = MemoryManager::getInstance();
		size_t binSize = binString->size();
		size_t hexSize = binSize * 2 + 1;
		
		MemoryBin* hex = mm->getMemory(hexSize);
		memset(*hex, 0, hexSize);

		size_t resultBinSize = 0;
		sodium_bin2hex(*hex, hexSize, (const unsigned char*)binString->data(), binSize);
		binString->assign((const char *)*hex, hexSize - 1);

		mm->releaseMemory(hex);
		return binString;
	}

	std::string pubkeyToHex(const unsigned char* pubkey)
	{
		auto mm = MemoryManager::getInstance();
		size_t hexSize = crypto_sign_PUBLICKEYBYTES * 2 + 1;
		size_t binSize = crypto_sign_PUBLICKEYBYTES;

		MemoryBin* hex = mm->getMemory(hexSize);
		memset(*hex, 0, hexSize);

		size_t resultBinSize = 0;

		sodium_bin2hex(*hex, hexSize, pubkey, binSize);

		std::string hexString((const char*)*hex, hexSize-1);
		mm->releaseMemory(hex);
		return hexString;
	}

	std::string timePointToString(const time_point<system_clock>& tp) 
	{
		// Convert time_point to time_t, which represents the calendar time
		std::time_t time = system_clock::to_time_t(tp);

		// Create a stream to hold the formatted date-time string
		std::stringstream ss;
		ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");

		return ss.str();
	}

	std::string timespanToString(const std::chrono::seconds timespan)
	{
		auto days = duration_cast<duration<int, std::ratio<86400>>>(timespan);
		auto hours = duration_cast<std::chrono::hours>(timespan % std::chrono::duration<int, std::ratio<86400>>(1));
		auto minutes = duration_cast<std::chrono::minutes>(timespan % std::chrono::hours(1));
		auto seconds = duration_cast<std::chrono::seconds>(timespan % std::chrono::minutes(1));

		std::ostringstream fmt;
		if (days.count() != 0) {
			fmt << days.count() << " days ";
		}
		if (hours.count() != 0) {
			fmt << hours.count() << " hours ";
		}
		if (minutes.count() != 0) {
			fmt << minutes.count() << " minutes ";
		}
		if (seconds.count() != 0) {
			fmt << seconds.count() << " seconds ";
		}

		return fmt.str();
	}
	using namespace std::chrono;
	const time_point<system_clock> convertFromProtoTimestamp(const proto::gradido::Timestamp& timestamp)
	{
		// Convert the seconds and nanoseconds to microseconds
		int64_t microseconds = timestamp.seconds() * static_cast<int64_t>(1e6) + timestamp.nanos() / static_cast<int64_t>(1e3);
		return system_clock::time_point(std::chrono::microseconds(microseconds));
	}
	void convertToProtoTimestamp(const time_point<system_clock> timestamp, proto::gradido::Timestamp* protoTimestamp)
	{
		// Convert time_point to duration since epoch
		auto duration = timestamp.time_since_epoch();

		// Convert duration to seconds and nanoseconds
		auto seconds = duration_cast<std::chrono::seconds>(duration);
		auto nanos = duration_cast<std::chrono::nanoseconds>(duration) - duration_cast<std::chrono::nanoseconds>(seconds);

		// Set the protobuf timestamp fields
		protoTimestamp->set_seconds(seconds.count());
		protoTimestamp->set_nanos(nanos.count());
	}
	time_point<system_clock> convertFromProtoTimestampSeconds(const proto::gradido::TimestampSeconds& timestampSeconds)
	{
		// Convert seconds to a duration
		auto seconds = std::chrono::seconds{ timestampSeconds.seconds() };

		// Create a time_point from the duration
		return system_clock::time_point{ seconds };
	}
	void convertToProtoTimestampSeconds(const std::chrono::time_point<std::chrono::system_clock> timestamp, proto::gradido::TimestampSeconds* protoTimestampSeconds)
	{
		// Get the duration since epoch
		auto duration = timestamp.time_since_epoch();

		// Convert the duration to seconds
		auto seconds = duration_cast<std::chrono::seconds>(duration);

		// Set the protobuf timestamp fields
		protoTimestampSeconds->set_seconds(seconds.count());
	}

	int replaceBase64WithHex(rapidjson::Value& json, rapidjson::Document::AllocatorType& alloc)
	{
		int count_replacements = 0;

		if (json.IsObject()) {
			for (auto it = json.MemberBegin(); it != json.MemberEnd(); it++) 
			{
				if (it->value.IsString()) {
					std::string name(it->name.GetString(), it->name.GetStringLength());
					if ("amount" == name || "finalGdd" == name) continue;
				}
				
				count_replacements += replaceBase64WithHex(it->value, alloc);
			}
		}
		else if (json.IsArray()) {
			for (auto it = json.Begin(); it != json.End(); it++) {
				count_replacements += replaceBase64WithHex(*it, alloc);
			}
		}
		else if (json.IsString()) 
		{
			std::string field_value(json.GetString(), json.GetStringLength());
			if (!std::regex_match(field_value, g_rexExpBase64)) return 0;

			auto bin = base64ToBin(field_value);
			if (!bin) return 0;

			auto mm = MemoryManager::getInstance();

			auto hex = binToHex(bin);
			mm->releaseMemory(bin);
			json.SetString(hex.data(), hex.size() - 1, alloc);
			return 1;
		}
		return count_replacements;
	}

	std::string replaceNewLineWithBr(std::string& in)
	{

		std::string::size_type pos = 0; // Must initialize
		while ((pos = in.find("\r\n", pos)) != std::string::npos) {
			in.replace(pos, 2, "<br>");
		}
		pos = 0;
		while ((pos = in.find("\n", pos)) != std::string::npos) {
			in.replace(pos, 1, "<br>");
		}
		pos = 0;
		while ((pos = in.find(" ", pos)) != std::string::npos) {
			in.replace(pos, 1, "&nbsp;");
		}
		return in;
	}

	// Exceptions
	InvalidHexException::InvalidHexException(const char* what) noexcept
		: GradidoBlockchainException(what)
	{

	}

	std::string InvalidHexException::getFullString() const
	{
		return std::string(what());
	}

	ParseNumberException::ParseNumberException(const char* what, NumberParseState parseState) noexcept
		: GradidoBlockchainException(what), mParseState(parseState)
	{

	}

	std::string ParseNumberException::getFullString() const
	{
		std::string result = numberParseStateToString(mParseState);
		result += ": ";
		result += what();
		return std::move(result);
	}
}


