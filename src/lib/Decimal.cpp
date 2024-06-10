#include "gradido_blockchain/lib/Decimal.h"
#include "gradido_blockchain/lib/Decay.h"

#ifndef USE_MPFR
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#endif 

Decimal::Decimal(const std::string& decimalString)
	: Decimal()
{
	fromString(decimalString.data());
}
/*
Decimal::Decimal(const memory::StringCachedAlloc& decimalString)
    : Decimal()
{
	fromString(decimalString.data());
}
*/
Decimal::Decimal(Decimal&& src) noexcept
	: mDecimal(std::move(src.mDecimal))
{
}

Decimal::Decimal()
{

}

Decimal::~Decimal()
{
}

#ifdef USE_MPFR
Decimal::Decimal(long number)
{
	mpfr_set_si(mDecimal, number, gDefaultRound);
}

Decimal::Decimal(const mpfr_ptr& decimal)
{
	mpfr_set(mDecimal, decimal, gDefaultRound);
}
Decimal::Decimal(const Decimal& src)
{
	mpfr_set(mDecimal, (mpfr_ptr)src, gDefaultRound);
}

std::string Decimal::toString(uint8_t precision /* = 25 */) const
{	
	char* tmpStr = nullptr;
	std::string formatString = "%." + std::to_string(precision) + "R*f";
	mpfr_asprintf(&tmpStr, formatString.data(), MPFR_RNDN, mDecimal.data());
	std::string result;
	result.assign(tmpStr);
	mpfr_free_str(tmpStr);
	return result;
}

void Decimal::fromString(const char* decimalString)
{
	if (mpfr_set_str(mDecimal, decimalString, 10, gDefaultRound)) {
		throw ParseDecimalStringException("cannot parse to decimal", decimalString);
	}
}

void Decimal::roundToPrecision(uint8_t precision)
{
	mpfr_exp_t exp;
	auto buffer = mpfr_get_str(nullptr, &exp, 10, 0, mDecimal, MPFR_RNDN);
	auto bufferSize = strlen(buffer);
	int firstRemovedZahl = buffer[precision + exp]-48;
	memset(&buffer[precision + exp], 0, bufferSize - precision - exp);
	std::string roundedDecimalString;
	roundedDecimalString.reserve(precision + exp + 3);
	if (exp < 1) {
		if (mDecimal.data()->_mpfr_sign == -1) {
			roundedDecimalString.assign("-0.");
		} else {
			roundedDecimalString.assign("0.");
		}
		roundedDecimalString.append(-exp, '0');
		roundedDecimalString.append(buffer, precision + exp);
	}
	mpfr_set_str(mDecimal, roundedDecimalString.data(), 10, gDefaultRound);
	if (firstRemovedZahl >= 5) {
		
	}

	mpfr_free_str(buffer);
}
#else 
Decimal::Decimal(long number)
	: mDecimal(static_cast<double>(number))
{
}

Decimal::Decimal(const double& decimal)
	: mDecimal(decimal)
{
}
Decimal::Decimal(const Decimal& src)
	: mDecimal(src.mDecimal)
{
}

std::string Decimal::toString(uint8_t precision /* = 25 */) const
{
	auto scale = pow(10.0, static_cast<double>(precision));
	std::stringstream ss;

	ss << std::fixed << std::setprecision(precision);	
	ss << std::round(mDecimal * scale) / scale;

	return ss.str();
}

void Decimal::fromString(const char* decimalString)
{
	mDecimal = std::stod(decimalString);
}

void Decimal::roundToPrecision(uint8_t precision)
{
	auto factor = std::pow(10, precision);
	mDecimal = std::round(mDecimal * factor) / factor;
}
#endif


// *************** Exceptions ************************
ParseDecimalStringException::ParseDecimalStringException(const char* what, const std::string& decimalString) noexcept
	: GradidoBlockchainException(what), mDecimalString(decimalString)
{

}

std::string ParseDecimalStringException::getFullString() const 
{
	std::string result = what();
	result += ", " + mDecimalString;
	return result;
}