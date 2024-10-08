#ifndef __GRADIDO_LOGIN_SERVER_LIB_DATA_TYPE_CONVERTER_H
#define __GRADIDO_LOGIN_SERVER_LIB_DATA_TYPE_CONVERTER_H

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/memory/Block.h"

#include "sodium.h"

#include "rapidjson/document.h"

#include <string>
#include "date/date.h"

namespace DataTypeConverter
{

	enum GRADIDOBLOCKCHAIN_EXPORT NumberParseState
	{
		NUMBER_PARSE_OKAY = 0,
		NUMBER_PARSE_INVALID_ARGUMENT,
		NUMBER_PARSE_OUT_OF_RANGE,
		NUMBER_PARSE_LOGIC_ERROR
	};

	GRADIDOBLOCKCHAIN_EXPORT NumberParseState strToInt(const std::string &input, int &result);
#ifdef __linux__
	GRADIDOBLOCKCHAIN_EXPORT NumberParseState strToInt(const std::string &input, unsigned long long &result);
#endif
	GRADIDOBLOCKCHAIN_EXPORT NumberParseState strToInt(const std::string &input, uint64_t &result);
	GRADIDOBLOCKCHAIN_EXPORT uint64_t strToInt(const std::string &input);
	GRADIDOBLOCKCHAIN_EXPORT NumberParseState strToDouble(const std::string &input, double &result);

	GRADIDOBLOCKCHAIN_EXPORT std::unique_ptr<std::string> hexToBinString(const std::string &hexString);
	GRADIDOBLOCKCHAIN_EXPORT inline std::string base64ToBinString(const std::string &base64String, int variant = sodium_base64_VARIANT_ORIGINAL)
	{
		memory::Block bin = memory::Block::fromBase64(base64String, variant);
		return bin.copyAsString();
	}
	GRADIDOBLOCKCHAIN_EXPORT std::string binToBase64(const unsigned char *data, size_t size, int variant = sodium_base64_VARIANT_ORIGINAL);
	GRADIDOBLOCKCHAIN_EXPORT inline std::string binToBase64(const std::string &proto_bin, int variant = sodium_base64_VARIANT_ORIGINAL)
	{
		return binToBase64((const unsigned char *)proto_bin.data(), proto_bin.size(), variant);
	}
	GRADIDOBLOCKCHAIN_EXPORT std::unique_ptr<std::string> binToBase64(std::unique_ptr<std::string> proto_bin, int variant = sodium_base64_VARIANT_ORIGINAL);

	GRADIDOBLOCKCHAIN_EXPORT std::string binToHex(const unsigned char *data, size_t size);
	GRADIDOBLOCKCHAIN_EXPORT std::unique_ptr<std::string> binToHex(std::unique_ptr<std::string> binString);
	GRADIDOBLOCKCHAIN_EXPORT inline std::string binToHex(const std::vector<unsigned char> &data) { return binToHex(data.data(), data.size()); }
	GRADIDOBLOCKCHAIN_EXPORT inline std::string binToHex(const std::string &str) { return binToHex((const unsigned char *)str.data(), str.size()); }

	//! \param pubkey pointer to array with crypto_sign_PUBLICKEYBYTES size
	GRADIDOBLOCKCHAIN_EXPORT std::string pubkeyToHex(const unsigned char *pubkey);

	GRADIDOBLOCKCHAIN_EXPORT const char *numberParseStateToString(NumberParseState state);
	//! \param fmt: https://howardhinnant.github.io/date/date.html#to_stream_formatting
	GRADIDOBLOCKCHAIN_EXPORT std::string timePointToString(const Timepoint &timepoint, const char *fmt = "%Y-%m-%d %H:%M:%S");
	//! \param fmt: https://howardhinnant.github.io/date/date.html#from_stream_formatting
	GRADIDOBLOCKCHAIN_EXPORT Timepoint dateTimeStringToTimePoint(const std::string &dateTimeString, const char *fmt = "%F %T");
	GRADIDOBLOCKCHAIN_EXPORT std::string timespanToString(const std::chrono::seconds timespan);
	GRADIDOBLOCKCHAIN_EXPORT Timepoint monthYearToTimepoint(const date::year_month& ym);

	//! \brief go through json object and replace every string entry in base64 format into hex format
	//! \return count of replaced strings
	GRADIDOBLOCKCHAIN_EXPORT int replaceBase64WithHex(rapidjson::Value &json, rapidjson::Document::AllocatorType &alloc);
	GRADIDOBLOCKCHAIN_EXPORT std::string replaceNewLineWithBr(std::string &in);

	class GRADIDOBLOCKCHAIN_EXPORT InvalidHexException : public GradidoBlockchainException
	{
	public:
		explicit InvalidHexException(const char *what) noexcept;

		std::string getFullString() const;
	};

	class GRADIDOBLOCKCHAIN_EXPORT ParseNumberException : public GradidoBlockchainException
	{
	public:
		explicit ParseNumberException(const char *what, NumberParseState parseState) noexcept;

		std::string getFullString() const;

	protected:
		NumberParseState mParseState;
	};
};

#endif // __GRADIDO_LOGIN_SERVER_LIB_DATA_TYPE_CONVERTER_H
