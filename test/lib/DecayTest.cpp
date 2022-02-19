
#include "gradidomath.h"
#include "gtest/gtest.h"

#include <chrono>

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
	mpfr_t decay_factor, gradido_decimal, temp;
	mpz_t  gradido_cent;
	mpfr_init(decay_factor); mpfr_init_set_ui(gradido_decimal, 0, MPFR_RNDN); mpfr_init(temp);
	mpz_init_set_ui(gradido_cent, 1000000);

	calculateDecayFactor(decay_factor, 365);
	calculateDecayFactorForDuration(decay_factor, decay_factor, 60 * 60 * 24 * 365);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	calculateDecayFast(decay_factor, gradido_decimal, gradido_cent, temp);
	
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
	char* str_decay = mpfr_get_str(NULL, &exp_temp, 10, 0, gradido_decimal, MPFR_RNDN);
	//printf("gradido_decimal: 0.%s\n", str_decay);
	// 99897064152173698	   
	ASSERT_STREQ(str_decay, "99897064152173698");
	mpfr_free_str(str_decay);

	//printf("gradido cent after: %d\n", cent_after);
	auto cent_after = mpz_get_ui(gradido_cent);
	ASSERT_EQ(499999, cent_after);
	mpfr_clear(decay_factor); mpfr_clear(gradido_decimal); mpfr_clear(temp);
	mpz_clear(gradido_cent);
}



TEST(gradido_math, calculate_decay)
{
	mpfr_t decay_factor;
	mpfr_init(decay_factor); 
	GradidoWithDecimal gradidos;
	gradidos.decimal = 0;
	gradidos.gradido = 1000000;

	calculateDecayFactor(decay_factor, 365);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	gradidos = calculateDecay(gradidos, 60 * 60 * 24 * 365, decay_factor);
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
	ASSERT_EQ(gradidos.decimal, 998971);
	ASSERT_EQ(gradidos.gradido, 499999);
	
	gradidos.gradido = 1000000;
	gradidos = calculateDecay(gradidos, 60 * 60 * 24, decay_factor);
	
	// after 1 day
	ASSERT_EQ(gradidos.decimal, 765722);
	ASSERT_EQ(gradidos.gradido, 998103);

	gradidos.gradido = 1000000;
	gradidos = calculateDecay(gradidos, 60 * 60 * 24 * 91, decay_factor);
	
	// after 91 days, roughly 3 months, 1/4 year
	ASSERT_EQ(gradidos.decimal, 376391);
	ASSERT_EQ(gradidos.gradido, 841296);
	
	gradidos.gradido = 1000000;
	gradidos = calculateDecay(gradidos, 60 * 60 * 24 * 182, decay_factor);
	
	// after 182 days, roughly 6 months, 1/2 year
	ASSERT_EQ(gradidos.decimal, 775407);
	ASSERT_EQ(gradidos.gradido, 707778);


	gradidos.gradido = 1000000;
	gradidos = calculateDecay(gradidos, 60 * 60 * 24 * 300, decay_factor);
	
	// after 300 days, roughly 10 months
	ASSERT_EQ(gradidos.decimal, 896082);
	ASSERT_EQ(gradidos.gradido, 565689);

	gradidos = calculateDecay(gradidos, 60 * 60 * 24 * 65, decay_factor);
	
	// after 1 year (accumulated)
	ASSERT_EQ(gradidos.decimal, 386674);
	ASSERT_EQ(gradidos.gradido, 500000);


	mpfr_clear(decay_factor);
}

//*/

int main(int argc, char** argv) 
{
//	void* memtest = malloc(100);
  //      free(memtest);
	::testing::InitGoogleTest(&argc, argv);

	auto result = RUN_ALL_TESTS();
	return result;
}

