#ifndef __GRADIDO_BLOCKCHAIN_LIB_DECIMAL_H
#define __GRADIDO_BLOCKCHAIN_LIB_DECIMAL_H

/*!
	@author einhornimmond
	@date 19.07.2022
	@brief Container for mpfr values, for easy convertion between string and mpfr
*/

#include "Decay.h"
#include <string>
#include "../GradidoBlockchainException.h"

class GRADIDOBLOCKCHAIN_EXPORT Decimal
{
public:
	//! throw ParseDecimalStringException if mpfr_set_str cannot parse the string to a number
	Decimal(const std::string& decimalString);
	Decimal(long number);
	// copy constructor
	Decimal(const mpfr_ptr& decimal);
	Decimal(const Decimal& decimal);
	// move constructor
	Decimal(Decimal&& src);
	Decimal();
	virtual ~Decimal();

	inline operator std::string() const { return toString(); }
	std::string toString(uint8_t precision = 25) const;
	void fromString(const std::string& decimalString);

	void roundToPrecision(uint8_t precision);

	inline void reset() { mpfr_set_ui(mDecimal, 0, gDefaultRound); }
	inline operator const mpfr_ptr() const { return mDecimal; }
	inline Decimal& operator+=(const Decimal& rhs) { mpfr_add(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator-=(const Decimal& rhs) { mpfr_sub(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator*=(const Decimal& rhs) { mpfr_mul(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator/=(const Decimal& rhs) { mpfr_div(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator=(const std::string& decimalString) { fromString(decimalString); return *this; }
	inline Decimal& operator=(const mpfr_ptr& decimal) { mpfr_set(mDecimal, decimal, gDefaultRound); return *this; }
	inline Decimal& operator=(const Decimal& b) { mpfr_set(mDecimal, b.mDecimal, gDefaultRound); return *this; }

protected:
	mpfr_ptr mDecimal;
};

inline Decimal operator+(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result += rhs;
	return std::move(result);
}

inline Decimal operator-(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result -= rhs;
	return std::move(result);
}
inline Decimal operator*(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result *= rhs;
	return std::move(result);
}
inline Decimal operator/(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result /= rhs;
	return std::move(result);
}

inline bool operator==(const Decimal& lhs, const Decimal& rhs) { return mpfr_cmp(lhs, rhs) == 0; }
inline bool operator!=(const Decimal& lhs, const Decimal& rhs) { return !operator==(lhs, rhs); }

/*!
	@author einhornimmond
	@date 19.07.2022
	@brief cache string representation of decimal
*/
class CachedDecimal : public Decimal
{
public:
	CachedDecimal(const std::string& decimalString);
	CachedDecimal(mpfr_ptr decimal);
	CachedDecimal();
	~CachedDecimal();

	operator std::string() const;
protected:
	bool mDirty;
	std::string mDecimalString;
};

class GRADIDOBLOCKCHAIN_EXPORT ParseDecimalStringException : public GradidoBlockchainException
{
public: 
	explicit ParseDecimalStringException(const char* what, const std::string& decimalString) noexcept;
	~ParseDecimalStringException() {}
	std::string getFullString() const;
protected:
	std::string mDecimalString;

};

#endif //__GRADIDO_BLOCKCHAIN_LIB_DECIMAL_H
