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

#include "gradido_blockchain/memory/VectorCacheAllocator.h"

#ifdef USE_MPFR
#include "gradido_blockchain/memory/MPFRBlock.h"
#endif //USE_MPFR

class GRADIDOBLOCKCHAIN_EXPORT Decimal
{
public:
	//! throw ParseDecimalStringException if mpfr_set_str cannot parse the string to a number
	Decimal(const std::string& decimalString);
	// Decimal(const memory::StringCachedAlloc& decimalString);
	Decimal(long number);
	Decimal(double decimal);
	// copy constructor
#ifdef USE_MPFR
	Decimal(const mpfr_ptr& decimal);
#endif // USE_MPFR
	Decimal(const Decimal& decimal);
	// move constructor
	Decimal(Decimal&& src) noexcept;
	Decimal();
	virtual ~Decimal();

	inline operator std::string() const { return toString(); }
	std::string toString(uint8_t precision = 25) const;
	void fromString(const char* decimalString);

	void roundToPrecision(uint8_t precision);
	inline Decimal& operator=(const std::string& decimalString) { fromString(decimalString.data()); return *this; }
#ifdef USE_MPFR
	inline void reset() { mpfr_set_ui(mDecimal, 0, gDefaultRound); }
	inline operator const mpfr_ptr() const { return mDecimal.data(); }
	inline operator const mpfr_srcptr() const { return mDecimal.data(); }
	inline Decimal& operator+=(const Decimal& rhs) { mpfr_add(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator+=(const int& b) { mpfr_add_si(mDecimal, mDecimal, b, gDefaultRound); return *this; }
	inline Decimal& operator-=(const Decimal& rhs) { mpfr_sub(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator-=(const int& b) { mpfr_sub_si(mDecimal, mDecimal, b, gDefaultRound); return *this; }
	inline Decimal& operator*=(const Decimal& rhs) { mpfr_mul(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator*=(const int& b) { mpfr_mul_si(mDecimal, mDecimal, b, gDefaultRound); return *this; }
	inline Decimal& operator/=(const Decimal& rhs) { mpfr_div(mDecimal, mDecimal, rhs, gDefaultRound); return *this; }
	inline Decimal& operator/=(const int& b) { mpfr_div_si(mDecimal, mDecimal, b, gDefaultRound); return *this; }
	inline Decimal& operator=(const mpfr_ptr& decimal) { mpfr_set(mDecimal, decimal, gDefaultRound); return *this; }
	inline Decimal& operator=(const Decimal& b) { mpfr_set(mDecimal, b.mDecimal.data(), gDefaultRound); return *this; }
	inline bool operator>(int b) { return mpfr_cmp_si(mDecimal, b) > 0; };
	inline bool operator<(int b) { return mpfr_cmp_si(mDecimal, b) < 0; };
#else
	inline void reset() { mDecimal = 0; }
	inline operator const double() const { return mDecimal; }
	inline Decimal& operator+=(const Decimal& rhs) { mDecimal += rhs; return *this; }
	inline Decimal& operator+=(const int& b) { mDecimal += static_cast<double>(b); return *this; }
	inline Decimal& operator-=(const Decimal& rhs) { mDecimal -= rhs; return *this; }
	inline Decimal& operator-=(const int& b) { mDecimal -= static_cast<double>(b); return *this; }
	inline Decimal& operator*=(const Decimal& rhs) { mDecimal *= rhs; return *this; }
	inline Decimal& operator*=(const int& b) { mDecimal *= static_cast<double>(b); return *this; }
	inline Decimal& operator/=(const Decimal& rhs) { mDecimal /= rhs; return *this; }
	inline Decimal& operator/=(const int& b) { mDecimal /= static_cast<double>(b); return *this; }
	inline Decimal& operator=(const double& decimal) { mDecimal = decimal; return *this; }
	inline Decimal& operator=(const Decimal& b) { mDecimal = b.mDecimal; return *this; }
	inline bool operator>(int b) { return mDecimal > static_cast<double>(b); };
	inline bool operator<(int b) { return mDecimal < static_cast<double>(b); };
#endif // USE_MPFR

protected:
#ifdef USE_MPFR
	 memory::MPFRBlock mDecimal;
#else 
	double mDecimal;
#endif //USE_MPFR
};
#ifdef USE_MPFR
inline Decimal operator+(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result += rhs;
	return result;
}

inline Decimal operator-(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result -= rhs;
	return result;
}

inline Decimal operator*(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result *= rhs;
	return result;
}

inline Decimal operator/(Decimal lhs, const Decimal& rhs)
{
	Decimal result;
	mpfr_set((mpfr_ptr)result, (mpfr_ptr)lhs, gDefaultRound);
	result /= rhs;
	return result;
}

inline Decimal operator ^(unsigned long lhs, const Decimal& rhs) {
	Decimal result;
	mpfr_ui_pow(result, lhs, rhs, gDefaultRound);
	return result;
}

inline bool operator==(const Decimal& lhs, const Decimal& rhs) { return mpfr_cmp(lhs, rhs) == 0; }
#else
inline Decimal operator+(Decimal lhs, const Decimal& rhs) { return static_cast<double>(lhs) + static_cast<double>(rhs); }
inline Decimal operator-(Decimal lhs, const Decimal& rhs) { return static_cast<double>(lhs) - static_cast<double>(rhs); }
inline Decimal operator*(Decimal lhs, const Decimal& rhs) { return static_cast<double>(lhs) * static_cast<double>(rhs); }
inline Decimal operator/(Decimal lhs, const Decimal& rhs) { return static_cast<double>(lhs) / static_cast<double>(rhs); }
inline Decimal operator ^(unsigned long lhs, const Decimal& rhs) { return pow(static_cast<double>(lhs), static_cast<double>(rhs)); }
inline bool operator==(const Decimal& lhs, const Decimal& rhs) { return static_cast<double>(lhs) == static_cast<double>(rhs);}

#endif //USE_MPFR
inline bool operator!=(const Decimal& lhs, const Decimal& rhs) { return !operator==(lhs, rhs); }


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
