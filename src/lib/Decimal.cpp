#include "gradido_blockchain/lib/Decimal.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/Decay.h"

Decimal::Decimal(const std::string& decimalString)
	: Decimal()
{
	fromString(decimalString);
}


Decimal::Decimal(long number)
	: Decimal()
{
	mpfr_set_si(mDecimal, number, gDefaultRound);
}

Decimal::Decimal(const mpfr_ptr& decimal)
	: Decimal()
{
	mpfr_set(mDecimal, decimal, gDefaultRound);
}
Decimal::Decimal(const Decimal& src)
	: Decimal()
{
	mpfr_set(mDecimal, (mpfr_ptr)src, gDefaultRound);
}

Decimal::Decimal(Decimal&& src) noexcept
{
	mDecimal = src.mDecimal;
	src.mDecimal = nullptr;
}

Decimal::Decimal()
	: mDecimal(MemoryManager::getInstance()->getMathMemory())
{

}

Decimal::~Decimal()
{
	if (mDecimal) {
		MemoryManager::getInstance()->releaseMathMemory(mDecimal);
		mDecimal = nullptr;
	}	
}

std::string Decimal::toString(uint8_t precision /* = 25 */) const
{	
	char* tmpStr = nullptr;
	std::string formatString = "%." + std::to_string(precision) + "R*f";
	mpfr_asprintf(&tmpStr, formatString.data(), MPFR_RNDN, mDecimal);
	std::string result;
	result.assign(tmpStr);
	mpfr_free_str(tmpStr);
	return std::move(result);
}

void Decimal::fromString(const std::string& decimalString)
{
	if (mpfr_set_str(mDecimal, decimalString.data(), 10, gDefaultRound)) {
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
		if (mDecimal->_mpfr_sign == -1) {
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