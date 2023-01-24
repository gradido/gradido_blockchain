#ifndef _GRADIDO_MATH_H
#define _GRADIDO_MATH_H

#include <mpfr.h>
#include "Poco/DateTime.h"

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

GRADIDOBLOCKCHAIN_EXPORT extern mpfr_t gDecayFactor356Days;
GRADIDOBLOCKCHAIN_EXPORT extern mpfr_t gDecayFactor366Days;
GRADIDOBLOCKCHAIN_EXPORT extern mpfr_t gDecayFactorGregorianCalender;
GRADIDOBLOCKCHAIN_EXPORT extern Poco::Timestamp DECAY_START_TIME;
GRADIDOBLOCKCHAIN_EXPORT extern const mpfr_rnd_t gDefaultRound;
GRADIDOBLOCKCHAIN_EXPORT void initDefaultDecayFactors();
GRADIDOBLOCKCHAIN_EXPORT void unloadDefaultDecayFactors();

// 1 second = 1×10^9 nanoseconds


//! \param decay_factor to store the result, decay factor per second
//! \param days_per_year 365 days in normal year, 366 days in leap year
//! \brief calculate decay factor per second for exponential decay calculation, needs 100-400 micro seconds depending on processor
/*!
 *  calculate decay factor with compound interest formula converted to q <br>
 *  n = (lg Kn - lg K0) / lg q => <br>
 *  lg q = (lg Kn - lg K0) / n => <br>
 *  q = e^((lg Kn - lg K0) / n)   <br>
 * <br>
 * with:
 * <ul>
 *  <li>q = decay_factor</li>
 *  <li>n = days_per_year * 60 * 60 * 24 = seconds per year</li>
 *  <li>Kn = 50 (capital after a year)</li>
 *  <li>K0 = 100 (capital at start)</li>
 * </ul>
 * 
 */
GRADIDOBLOCKCHAIN_EXPORT void calculateDecayFactor(mpfr_ptr decay_factor, int days_per_year);

//! \param decay_for_duration store the result, decay factor for specific duration
//! \param decay_factor per second from ::calculateDecayFactor
//! \param seconds duration in seconds
//! \brief calculate decay_for_duration = decay_factor^seconds
/*!
 * calculate the decay factor for the specific duration
 */
//GRADIDOBLOCKCHAIN_EXPORT void calculateDecayFactorForDuration(mpfr_ptr decay_for_duration, mpfr_ptr decay_factor, unsigned long seconds);

//! \param decay_for_duration store the result, decay factor for specific duration
//! \param decay_factor per second from ::calculateDecayFactor
//! \param startTime from which the duration calculation should begins, if it is not older than the start of gradido decay
//! \param endTime until when the duration lasts if it is younger as the start of gradido decay
//! \brief calculate decay_for_duration = decay_factor^(end - start)
/*!
 * calculate the decay factor for the specific duration, taking into account the gradido decay start
 * start = max(startTime, DECAY_START_TIME)
 * end = max(endTime, DECAY_START_TIME)
 */
GRADIDOBLOCKCHAIN_EXPORT void calculateDecayFactorForDuration(
	mpfr_ptr decay_for_duration, mpfr_ptr decay_factor,
	Poco::Timestamp startTime, Poco::Timestamp endTime
);

GRADIDOBLOCKCHAIN_EXPORT void calculateDecayFactorForDuration(
	mpfr_ptr decay_for_duration, mpfr_ptr decay_factor,
	unsigned long durationSeconds
);

GRADIDOBLOCKCHAIN_EXPORT Poco::Timespan calculateDecayDurationSeconds(Poco::Timestamp startTime, Poco::Timestamp endTime);

inline void calculateDecayFactorForDuration(
	mpfr_ptr decay_for_duration, mpfr_ptr decay_factor,
	Poco::DateTime startTime, Poco::DateTime endTime
) { calculateDecayFactorForDuration(decay_for_duration, decay_factor, startTime.timestamp(), endTime.timestamp());}

//! \param decay_for_duration decay factor for specific duration, taken from ::calculateDecayFactorForDuration
//! \param gradido gradido decimal, contain updated value after function call
//! \brief calculate decayed balance without memory allocation
/*!
 * calculate decay with large precision: <br>
 * temp = gradido * decay_for_duration <br>
 */
GRADIDOBLOCKCHAIN_EXPORT void calculateDecayFast(mpfr_ptr decay_for_duration, mpfr_ptr gradido);

//GRADIDOBLOCKCHAIN_EXPORT void calculateDecay(const mpfr_ptr decay_factor, unsigned long seconds, mpfr_ptr gradido);

class GRADIDOBLOCKCHAIN_EXPORT ParseStringToMpfrException : public GradidoBlockchainException
{
public:
	explicit ParseStringToMpfrException(const char* what, const std::string& valueString) noexcept;

	std::string getFullString() const;

protected:
	std::string mValueString;
};


#endif //_GRADIDO_MATH_H