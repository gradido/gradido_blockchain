#ifndef __GRADIDO_BLOCKCHAIN_GRADIDO_UNI_H
#define __GRADIDO_BLOCKCHAIN_GRADIDO_UNI_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include "r128.h"

#include <typeinfo>
#include <string>

class GRADIDOBLOCKCHAIN_EXPORT GradidoUnit
{
public:
	GradidoUnit() : mGradidoPrecise(0) {}
	//! will be rounded to maximal 4 after comma so 1.271827 will be 1.2718
	GradidoUnit(double gdd) : mGradidoPrecise(gdd) {};
	GradidoUnit(R128 gddPrecise) : mGradidoPrecise(gddPrecise) {};
  ~GradidoUnit() {};

	//! expect decimal string, like 101.1728
	static GradidoUnit fromString(const std::string& stringAmount);
	//! will be understood as gdd cent with 4 after comma, so gdd = gddCent / 10000
	static GradidoUnit fromGradidoCent(int64_t gddCent) { return GradidoUnit(R128(gddCent) / R128(10000)); }

	GradidoUnit roundToPrecision(uint8_t precision = 4) const;

//! \param precision expect value in the range [0;4]
	std::string toString(int precision = 4) const;
	inline int64_t getGradidoCent() const;

	// access operators
	inline operator std::string() const { return toString(); }
	inline operator double() const { return roundToPrecisionDouble(mGradidoPrecise, 4); }

	// math in place operators
	inline GradidoUnit& operator+=(const GradidoUnit& other) { mGradidoPrecise += other.mGradidoPrecise; return *this; }
	inline GradidoUnit& operator-=(const GradidoUnit& other) { mGradidoPrecise -= other.mGradidoPrecise; return *this; }
	// inline GradidoUnit& operator*=(const GradidoUnit& other) { mGradidoPrecise *= other.mGradidoPrecise; return *this; }

	inline GradidoUnit& operator=(const GradidoUnit& other) { mGradidoPrecise = other.mGradidoPrecise; return *this; }

	// math operators
	inline GradidoUnit operator+(const GradidoUnit& other) const { return mGradidoPrecise + other.mGradidoPrecise;}
	inline GradidoUnit operator-(const GradidoUnit& other) const { return mGradidoPrecise - other.mGradidoPrecise; }
	// inline GradidoUnit operator*(const GradidoUnit& other) const { return mGradidoPrecise * other.mGradidoPrecise; }

	// compare
	inline bool operator>(const GradidoUnit& other) const { return mGradidoPrecise > other.mGradidoPrecise; }
	inline bool operator>=(const GradidoUnit& other) const { return mGradidoPrecise >= other.mGradidoPrecise; }
	inline bool operator<(const GradidoUnit& other) const { return mGradidoPrecise < other.mGradidoPrecise; }
	inline bool operator<=(const GradidoUnit& other) const { return mGradidoPrecise <= other.mGradidoPrecise; }
	inline bool operator==(const GradidoUnit& other) const { return mGradidoPrecise == other.mGradidoPrecise; }
	inline bool operator!=(const GradidoUnit& other) const { return mGradidoPrecise != other.mGradidoPrecise; }

	// negate, flip sign
	inline GradidoUnit& negate() { mGradidoPrecise = -mGradidoPrecise; return *this;}
	inline GradidoUnit negated() const { return GradidoUnit(-mGradidoPrecise); }

    //! decay calculation
	GradidoUnit calculateDecay(int64_t seconds) const;
	inline GradidoUnit calculateDecay(Duration duration) const;
	inline GradidoUnit calculateDecay(Timepoint startTime, Timepoint endTime) const;

	//! reverse decay calculation or original compound interest calculation
	inline GradidoUnit calculateCompoundInterest(Duration duration) const;
	inline GradidoUnit calculateCompoundInterest(Timepoint startTime, Timepoint endTime) const;

	//! be aware that in production we started with decay calculation at a specific date
	static Duration calculateDecayDurationSeconds(Timepoint startTime, Timepoint endTime);
  static GradidoUnit zero() { return GradidoUnit(); }

protected:
	static double roundToPrecisionDouble(R128 GradidoUnit, uint8_t precision);

	R128 mGradidoPrecise;
};

int64_t GradidoUnit::getGradidoCent() const
{
	R128 result = mGradidoPrecise * R128(10000);
	if (result.hi > INT64_MAX) {
		throw FixedPointedArithmetikOverflowException("overflow in get gradido Cent", result.hi);
	}
	return result.hi;
}

GradidoUnit GradidoUnit::calculateCompoundInterest(Duration duration) const {
	return calculateDecay(-std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}
GradidoUnit GradidoUnit::calculateCompoundInterest(Timepoint startTime, Timepoint endTime) const {
	return calculateCompoundInterest(calculateDecayDurationSeconds(startTime, endTime));
}
GradidoUnit GradidoUnit::calculateDecay(Duration duration) const {
	return calculateDecay(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}
GradidoUnit GradidoUnit::calculateDecay(Timepoint startTime, Timepoint endTime) const {
	return calculateDecay(calculateDecayDurationSeconds(startTime, endTime));
}

#endif //__GRADIDO_BLOCKCHAIN_GRADIDO_UNI_H
