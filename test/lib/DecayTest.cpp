
#include "gradido_blockchain/lib/Decay.h"
#include "gradido_blockchain/MemoryManager.h"
#include "gtest/gtest.h"

#include <chrono>

std::string mpfr_to_string(mpfr_ptr value)
{
	mpfr_exp_t exp_temp;
	std::string result;
	char* str = mpfr_get_str(NULL, &exp_temp, 10, 0, value, MPFR_RNDN);
	result = str;
	result += ", exp: " + std::to_string(exp_temp);
	
	mpfr_free_str(str);
	return result;
}

TEST(gradido_math, test_test)
{

}

TEST(gradido_math, calculate_decay_factor)
{
	mpfr_t decay_365, decay_366, temp;
	mpfr_init(decay_365); mpfr_init(decay_366); mpfr_init(temp);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	calculateDecayFactor(decay_365, 365);
	calculateDecayFactor(decay_366, 366);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << "[ DURATION ] ";
	if (duration.count() > 1100) {
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << " micro s" << std::endl;
	}
	else {
		std::cout << duration.count() << " ns" << std::endl;
	}

	// print string
	mpfr_exp_t exp_temp;
	char* str_decay = mpfr_get_str(NULL, &exp_temp, 10, 0, decay_365, MPFR_RNDN);
	
	//99999997802044727
	ASSERT_STREQ(str_decay, "99999997802044727");
	//printf("decay factor for 365 days: 0.%s\n", str_decay);
	mpfr_free_str(str_decay);

	str_decay = mpfr_get_str(NULL, &exp_temp, 10, 0, decay_366, MPFR_RNDN);
	//printf("decay factor for 366 days: 0.%s\n", str_decay);
	//99999997808050067
	ASSERT_STREQ(str_decay, "99999997808050067");
	mpfr_free_str(str_decay);

	mpfr_clear(decay_365); mpfr_clear(decay_366); mpfr_clear(temp);

}

TEST(gradido_math, calculate_decay_factor_for_duration)
{
	mpfr_t decay_factor, temp;
	mpfr_init(decay_factor); mpfr_init(temp);

	calculateDecayFactor(decay_factor, 365);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	calculateDecayFactorForDuration(decay_factor, decay_factor, 60*60*24*365);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << "[ DURATION ] ";
	if (duration.count() > 1100) {
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << " micro s" << std::endl;
	}
	else {
		std::cout << duration.count() << " ns" << std::endl;
	}

	// print string
	mpfr_exp_t exp_temp;
	char* str_decay = mpfr_get_str(NULL, &exp_temp, 10, 0, decay_factor, MPFR_RNDN);
	//printf("decay factor for 1 day (365 days in year): 0.%s\n", str_decay);
	//49999999897064151
	ASSERT_STREQ(str_decay, "49999999897064151");
	mpfr_free_str(str_decay);

	mpfr_clear(decay_factor); mpfr_clear(temp);
}

TEST(gradido_math, calculate_decay_fast)
{
	mpfr_t decay_factor, gradido;
	mpfr_init(decay_factor); 
	mpfr_init_set_ui(gradido, 100, MPFR_RNDN);

	calculateDecayFactor(decay_factor, 365);
	calculateDecayFactorForDuration(decay_factor, decay_factor, 60 * 60 * 24 * 365);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	calculateDecayFast(decay_factor, gradido);
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << "[ DURATION ] ";
	if (duration.count() > 1100) {
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << " micro s" << std::endl;
	}
	else {
		std::cout << duration.count() << " ns" << std::endl;
	}

	// print string
	mpfr_exp_t exp_temp;
	char* str_decay = mpfr_get_str(NULL, &exp_temp, 10, 0, gradido, MPFR_RNDN);
	printf("gradido: 0.%s\n", str_decay);
	// 49999999897064150	   
	EXPECT_STREQ(str_decay, "49999999897064150");
	mpfr_free_str(str_decay);

	mpfr_clear(decay_factor);
	mpfr_clear(gradido);
}

/*
* calculateDecay(const mpfr_ptr decay_factor, unsigned long seconds, mpfr_ptr gradido)
*/

TEST(gradido_math, calculate_decay)
{
	mpfr_t decay_factor, gradido;
	mpfr_init2(decay_factor, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
	mpfr_init2(gradido, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
	mpfr_set_ui(gradido, 100, MPFR_RNDN);

	calculateDecayFactor(decay_factor, 365);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	calculateDecay(decay_factor, 60 * 60 * 24 * 365, gradido);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	std::cout << "[ DURATION ] ";
	if (duration.count() > 1100) {
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(duration).count() << " micro s" << std::endl;
	}
	else {
		std::cout << duration.count() << " ns" << std::endl;
	}

	// after 1 year
	auto str = mpfr_to_string(gradido);
	//printf("after 1 year: %s\n", str.data());
	EXPECT_EQ(mpfr_to_string(gradido), "4999999999999999999999999999999685845636, exp: 2");
	
	// after 1 day
	mpfr_set_ui(gradido, 100, MPFR_RNDN);
	calculateDecay(decay_factor, 60 * 60 * 24, gradido);	
	EXPECT_EQ(mpfr_to_string(gradido), "9981027686515946342007432616571981246170, exp: 2");

	// after 91 days, roughly 3 months, 1/4 year
	mpfr_set_ui(gradido, 100, MPFR_RNDN);	
	calculateDecay(decay_factor, 60 * 60 * 24 * 91, gradido);	
	EXPECT_EQ(mpfr_to_string(gradido), "8412957326244545352994310926122792051017, exp: 2");
	
	// after 182 days, roughly 6 months, 1/2 year
	mpfr_set_ui(gradido, 100, MPFR_RNDN);
	calculateDecay(decay_factor, 60 * 60 * 24 * 182, gradido);	
	EXPECT_EQ(mpfr_to_string(gradido), "7077785097321176951408529071416428845829, exp: 2");

	// after 300 days, roughly 10 months
	mpfr_set_ui(gradido, 100, MPFR_RNDN);
	calculateDecay(decay_factor, 60 * 60 * 24 * 300, gradido);	
	EXPECT_EQ(mpfr_to_string(gradido), "5656894583997203205263376978432563916949, exp: 2");

	// after 1 year (accumulated)
	calculateDecay(decay_factor, 60 * 60 * 24 * 65, gradido);
	EXPECT_EQ(mpfr_to_string(gradido), "4999999999999999999999999999999685845636, exp: 2");

	mpfr_clear(decay_factor);
	mpfr_clear(gradido);
}
