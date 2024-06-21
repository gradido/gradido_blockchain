#include "date/date.h"
#include "gradido_blockchain/lib/Decay.h"

#include "gradido_blockchain/memory/MPFRBlock.h"


// Fixed-Point-Arithmetic Code by ChatGPT 4.0
// Fixed-point precision scaling factor
constexpr long long SCALE = 1LL << 32;
constexpr long long SECONDS_PER_YEAR = 31556952;
constexpr double LOG2 = 0.6931471805599453; // log(2)

// Function to calculate 2^x using fixed-point arithmetic
long long fixed_point_exp2(long long x)
{
	long long result = SCALE;
	long long term = SCALE;
	for (int i = 1; i < 20; ++i) {
		term = (term * x) / (SCALE * i);
		result += term;
		// Check for potential overflow
		if (term == 0) break;
	}
	return result;
}

long long decayFormula(long long value, unsigned long seconds)
{
	// Convert duration to a fixed-point time factor
	long long timeFactor = (seconds * SCALE) / SECONDS_PER_YEAR;
	timeFactor = (timeFactor * LOG2 * SCALE) / SCALE; // Multiply by log(2)

	// Compute 2^timeFactor in fixed-point
	long long decayFactor = fixed_point_exp2(timeFactor);

	// Perform division in fixed-point and rescale
	return (value * SCALE) / decayFactor;
}

/*
typedef enum {
MPFR_RNDN=0,  // round to nearest, with ties to even
MPFR_RNDZ,    // round toward zero
MPFR_RNDU,    // round toward +Inf
MPFR_RNDD,    // round toward -Inf
MPFR_RNDA,    // round away from zero
MPFR_RNDF,    // faithful rounding
MPFR_RNDNA = -1 // round to nearest, with ties away from zero (mpfr_round)
} mpfr_rnd_t;

*/
#ifdef USE_MPFR
// TODO: move to server config
const mpfr_rnd_t gDefaultRound = MPFR_RNDA;

mpfr_t gDecayFactor356Days;
mpfr_t gDecayFactor366Days;
mpfr_t gDecayFactorGregorianCalender;
#else 
long double gDecayFactorApollo = 0.99999997803504048973201202316767079413460520837376;
#endif
Timepoint DECAY_START_TIME;

bool gInited = false;

void initDefaultDecayFactors()
{
	gInited = true;
#ifdef USE_MPFR
	mpfr_init2(gDecayFactor356Days, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
	calculateDecayFactor(gDecayFactor356Days, 356);
	mpfr_init2(gDecayFactor366Days, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
	calculateDecayFactor(gDecayFactor366Days, 366);

	// calculate decay factor with Gregorian Calender
	// 365.2425 days per year
	mpfr_t temp;
	mpfr_init2(temp, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
	mpfr_init2(gDecayFactorGregorianCalender, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
	// (lg Kn - lg K0) / seconds in year
	mpfr_log_ui(temp, 50, gDefaultRound);
	mpfr_log_ui(gDecayFactorGregorianCalender, 100, gDefaultRound);
	mpfr_sub(temp, temp, gDecayFactorGregorianCalender, gDefaultRound);

	mpfr_set_d(gDecayFactorGregorianCalender, 365.2425 * 24.0 * 60.0 * 60.0, gDefaultRound);

	mpfr_div(gDecayFactorGregorianCalender, temp, gDecayFactorGregorianCalender, gDefaultRound);

	// precision error in advantage for user
	mpfr_exp(gDecayFactorGregorianCalender, gDecayFactorGregorianCalender, MPFR_RNDZ);

	mpfr_clear(temp);
	// set decay factor manuell to the same value like the apollo server use it
	//std::string apolloDecayValueString = "0.99999997803504048973201202316767079413460520837376";
	//std::string apolloDecayValueString = "0.9999999780350404897320120"; 
	//mpfr_set_str(gDecayFactorGregorianCalender, apolloDecayValueString.data(), 10, MPFR_RNDZ);
#endif
	std::string decayStartTimeString = "2021-05-13 17:46:31";
/*	
	int timezoneDifferential = Poco::DateTimeFormatter::UTC; // + GMT 0
	DECAY_START_TIME = Poco::DateTimeParser::parse(
		Poco::DateTimeFormat::SORTABLE_FORMAT,
		decayStartTimeString,
		timezoneDifferential 
	).timestamp(); 
	*/

	std::istringstream in{ decayStartTimeString };
	date::sys_seconds tp;
	in >> date::parse("%F %T", tp);	
	DECAY_START_TIME = tp;
}

void unloadDefaultDecayFactors()
{
	if (!gInited) return;
#ifdef USE_MPFR
	mpfr_clear(gDecayFactor356Days);
	mpfr_clear(gDecayFactor366Days);
	mpfr_clear(gDecayFactorGregorianCalender);
#endif
	gInited = false;
}
#ifdef USE_MPFR
void calculateDecayFactor(mpfr_ptr decay_factor, int days_per_year)
{
	assert(gInited && "Decay lib not initalized");
	mpfr_t temp;
	mpfr_init2(temp, decay_factor->_mpfr_prec);
	// (lg Kn - lg K0) / seconds in year
	mpfr_log_ui(temp, 50, gDefaultRound);
	mpfr_log_ui(decay_factor, 100, gDefaultRound);
	mpfr_sub(temp, temp, decay_factor, gDefaultRound);
	
	mpfr_set_ui(decay_factor, days_per_year * 60 * 60 * 24, gDefaultRound);
	
	mpfr_div(decay_factor, temp, decay_factor, gDefaultRound);
	
	// precision error in advantage for user
	mpfr_exp(decay_factor, decay_factor, MPFR_RNDZ);
	
	mpfr_clear(temp);
}

void calculateDecayFactorForDuration(mpfr_ptr decay_for_duration, mpfr_ptr decay_factor, Duration duration)
{
	assert(gInited && "Decay lib not initalized");
	auto seconds = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
	mpfr_pow_ui(decay_for_duration, decay_factor, seconds, gDefaultRound);
}

//max(a,b) (((a) > (b)) ? (a) : (b))

void calculateDecayFactorForDuration(
	mpfr_ptr decay_for_duration, mpfr_ptr decay_factor,
	Timepoint startTime, Timepoint endTime
)
{
	assert(gInited && "Decay lib not initalized");
	auto duration = static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::seconds>(calculateDecayDurationSeconds(startTime, endTime)).count());
	if (!duration) {
		// no decay
		mpfr_set_ui(decay_for_duration, 1, gDefaultRound);
	}
	else {
		mpfr_pow_ui(decay_for_duration, decay_factor, duration, gDefaultRound);
	}
}

void calculateDecayFast(mpfr_ptr decay_for_duration, mpfr_ptr gradido)
{
	assert(gInited && "Decay lib not initalized");
	// gradido * decay 
	mpfr_mul(gradido, gradido, decay_for_duration, gDefaultRound);
}

void calculateDecay(const mpfr_ptr decay_factor, unsigned long seconds, mpfr_ptr gradido)
{
	assert(gInited && "Decay lib not initalized");
	memory::MPFRBlock temp;
	mpfr_pow_ui(temp, decay_factor, seconds, gDefaultRound);
	mpfr_mul(gradido, gradido, temp, gDefaultRound);
}

mpfr_ptr decayFormula(mpfr_ptr value, unsigned long seconds)
{
	calculateDecay(gDecayFactorGregorianCalender, seconds, value);
	return value;
}

#endif
Duration calculateDecayDurationSeconds(Timepoint startTime, Timepoint endTime)
{
	assert(endTime > startTime);
	Timepoint start = startTime > DECAY_START_TIME ? startTime : DECAY_START_TIME;
	Timepoint end = endTime > DECAY_START_TIME ? endTime : DECAY_START_TIME;
	if (start == end) return std::chrono::seconds{0};
	return end - start;
}


long double decayFormula(long double value, unsigned long seconds)
{
	static long double factor = 0.99999997803504048973201202316767079413460520837376;
	printf("long double size: %lld\n", sizeof(factor));
	return roundl(value * powl(factor, static_cast<long double>(seconds)));
}

ParseStringToMpfrException::ParseStringToMpfrException(const char* what, const std::string& valueString) noexcept
	: GradidoBlockchainException(what), mValueString(valueString)
{

}

std::string ParseStringToMpfrException::getFullString() const
{
	return std::string(what()) + ", value string: " + mValueString;
}