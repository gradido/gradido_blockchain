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
	// will be understood as gdd cent with 4 after comma, so gdd = gddCent / 10000
	GradidoUnit(int64_t gddCent) : mGradidoCent(gddCent) {}
	// will be rounded to maximal 4 after comma so 1.271827 will be 1.2718
	GradidoUnit(double gdd);
	GradidoUnit(const std::string& stringAmount);
	
	~GradidoUnit() {};

	std::string toString() const;

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

	// comparisation
	inline bool operator>(const GradidoUnit& other) const { return mGradidoCent > other.mGradidoCent; }
	inline bool operator>=(const GradidoUnit& other) const { return mGradidoCent >= other.mGradidoCent; }
	inline bool operator<(const GradidoUnit& other) const { return mGradidoCent < other.mGradidoCent; }
	inline bool operator<=(const GradidoUnit& other) const { return mGradidoCent <= other.mGradidoCent; }
	inline bool operator==(const GradidoUnit& other) const { return mGradidoCent == other.mGradidoCent; }
	inline bool operator!=(const GradidoUnit& other) const { return mGradidoCent != other.mGradidoCent; }

	//! decay calculation with fixed point arithmetik for max grad of deterministic
	static int64_t calculateDecay(int64_t gradidoCent, Duration duration);
	inline GradidoUnit calculateDecay(Duration duration) const {
		return calculateDecay(mGradidoCent, duration);
	}
	inline GradidoUnit calculateDecay(Timepoint startTime, Timepoint endTime) const {
		return calculateDecay(calculateDecayDurationSeconds(startTime, endTime));
	}
	//! be aware that in production we started with decay calculation at a specific date
	static Duration calculateDecayDurationSeconds(Timepoint startTime, Timepoint endTime);
	
protected:	
	static double roundToPrecision(double GradidoUnit, uint8_t precision);

	int64_t mGradidoCent;
};


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