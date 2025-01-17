#ifndef __GRADIDO_BLOCKCHAIN_GRADIDO_UNI_H
#define __GRADIDO_BLOCKCHAIN_GRADIDO_UNI_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/types.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

#include <typeinfo>
#include <string>

class GRADIDOBLOCKCHAIN_EXPORT GradidoUnit
{
public:
	GradidoUnit() : mGradidoCent(0) {}
	//! will be rounded to maximal 4 after comma so 1.271827 will be 1.2718
	GradidoUnit(double gdd) : GradidoUnit(static_cast<int64_t>(roundToPrecision(gdd, 4) * 10000.0)) {};
	GradidoUnit(const std::string& stringAmount) : GradidoUnit(std::stod(stringAmount)) {};
    ~GradidoUnit() {};

	//! expect decimal string, like 101.1728
	static GradidoUnit fromString(const std::string& stringAmount) { return GradidoUnit(stringAmount); }
	//! will be understood as gdd cent with 4 after comma, so gdd = gddCent / 10000
	static GradidoUnit fromGradidoCent(int64_t gddCent) { return GradidoUnit(gddCent); }

//! \param precision expect value in the range [0;4]
	std::string toString(int precision = 4) const;
	inline int64_t getGradidoCent() const { return mGradidoCent; }

	// access operators
	inline operator std::string() const { return toString(); }
	inline operator double() const { return static_cast<double>(mGradidoCent) / 10000; }

	// math in place operators
	inline GradidoUnit& operator+=(const GradidoUnit& other) { mGradidoCent += other.mGradidoCent; return *this; }
	inline GradidoUnit& operator-=(const GradidoUnit& other) { mGradidoCent -= other.mGradidoCent; return *this; }
	inline GradidoUnit& operator*=(const GradidoUnit& other) { mGradidoCent *= other.mGradidoCent; return *this; }

	inline GradidoUnit& operator=(const GradidoUnit& other) { mGradidoCent = other.mGradidoCent; return *this; }

	// math operators
	inline GradidoUnit operator+(const GradidoUnit& other) const { return mGradidoCent + other.mGradidoCent;}
	inline GradidoUnit operator-(const GradidoUnit& other) const { return mGradidoCent - other.mGradidoCent; }
	inline GradidoUnit operator*(const GradidoUnit& other) const { return mGradidoCent * other.mGradidoCent; }

	// compare
	inline bool operator>(const GradidoUnit& other) const { return mGradidoCent > other.mGradidoCent; }
	inline bool operator>=(const GradidoUnit& other) const { return mGradidoCent >= other.mGradidoCent; }
	inline bool operator<(const GradidoUnit& other) const { return mGradidoCent < other.mGradidoCent; }
	inline bool operator<=(const GradidoUnit& other) const { return mGradidoCent <= other.mGradidoCent; }
	inline bool operator==(const GradidoUnit& other) const { return mGradidoCent == other.mGradidoCent; }
	inline bool operator!=(const GradidoUnit& other) const { return mGradidoCent != other.mGradidoCent; }

	// negate, flip sign
	inline GradidoUnit& negate() { mGradidoCent = -mGradidoCent; return *this;}
  inline GradidoUnit negated() const { return GradidoUnit(-mGradidoCent); }

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
    // will be understood as gdd cent with 4 after comma, so gdd = gddCent / 10000
    GradidoUnit(int64_t gddCent) : mGradidoCent(gddCent) {}
	static double roundToPrecision(double GradidoUnit, uint8_t precision);

	int64_t mGradidoCent;
};

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

/*!
* will be thrown if x
*/
class FixedPointedArithmetikOverflowException : public GradidoBlockchainException
{
public:
	explicit FixedPointedArithmetikOverflowException(const char* what, long long x) noexcept
		: GradidoBlockchainException(what), mX(x) {}

	std::string getFullString() const {
		std::string result = what();
		result += ", x: " + std::to_string(mX);
		return result;
	}
protected:
	long long mX;
};


#endif //__GRADIDO_BLOCKCHAIN_GRADIDO_UNI_H
