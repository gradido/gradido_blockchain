#include "gradido_blockchain/lib/Decay.h"

#include "gradido_blockchain/MemoryManager.h"
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
// TODO: move to server config
const mpfr_rnd_t gDefaultRound = MPFR_RNDN;

mpfr_t gDecayFactor356Days;
mpfr_t gDecayFactor366Days;
mpfr_t gDecayFactorGregorianCalender;

void initDefaultDecayFactors()
{
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
}

void unloadDefaultDecayFactors()
{
	mpfr_clear(gDecayFactor356Days);
	mpfr_clear(gDecayFactor366Days);
	mpfr_clear(gDecayFactorGregorianCalender);
}

void calculateDecayFactor(mpfr_ptr decay_factor, int days_per_year)
{
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

void calculateDecayFactorForDuration(mpfr_ptr decay_for_duration, mpfr_ptr decay_factor, unsigned long seconds)
{
	mpfr_pow_ui(decay_for_duration, decay_factor, seconds, gDefaultRound);
}

void calculateDecayFast(mpfr_ptr decay_for_duration, mpfr_ptr gradido)
{
	// gradido * decay 
	mpfr_mul(gradido, gradido, decay_for_duration, gDefaultRound);
}

void calculateDecay(const mpfr_ptr decay_factor, unsigned long seconds, mpfr_ptr gradido)
{
	auto mm = MemoryManager::getInstance();
	auto temp = mm->getMathMemory();
	mpfr_pow_ui(temp, decay_factor, seconds, gDefaultRound);
	mpfr_mul(gradido, gradido, temp, gDefaultRound);
	mm->releaseMathMemory(temp);
}