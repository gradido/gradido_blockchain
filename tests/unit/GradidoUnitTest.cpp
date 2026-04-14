#include "main.h"
#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/Profiler.h"
#include "gradido_blockchain/lib/MultithreadList.h"

#include <array>
#include <iomanip>
#include <random>

::testing::AssertionResult IsBetweenInclusive(int64_t val, int64_t a, int64_t b)
{
  if ((val >= a) && (val <= b))
    return ::testing::AssertionSuccess();
  else
    return ::testing::AssertionFailure()
    << val << " is outside the range " << a << " to " << b;
}

TEST(GradidoUnitTest, ConstructEmpty)
{
  auto defaultConstructor = GradidoUnit();
  EXPECT_EQ(defaultConstructor, GradidoUnit::zero());
  EXPECT_EQ(defaultConstructor.getGradidoCent(), 0);
}

TEST(GradidoUnitTest, ConstructWithDouble)
{
  auto doubleConstructor = GradidoUnit(0.10212);
  EXPECT_EQ(static_cast<double>(doubleConstructor), 0.1021);
  EXPECT_EQ(doubleConstructor.getGradidoCent(), 1021);
  EXPECT_EQ(doubleConstructor.toString(), "0.1021");
}

TEST(GradidoUnitTest, ConstructWithString)
{
  auto stringConstructor = GradidoUnit::fromString("271.2817261");
  EXPECT_EQ(static_cast<double>(stringConstructor), 271.2817);
  EXPECT_EQ(stringConstructor.toString(), "271.2817");
}


TEST(GradidoUnitTest, TestWithManyDifferentDuration)
{
  // 2^(x/31556952)
  // calculate for every 32. second until two years are full
  auto prevValue = GradidoUnit::zero();
  auto prevDistance = GradidoUnit::zero();
  for (int i = 1; i < 31556952 * 2; i+=32) {
    auto decayed = GradidoUnit::fromGradidoCent(100000000).calculateDecay(i);
    if (prevValue) {
      ASSERT_GE(prevValue, decayed) << "previous value wasn't greater on i: " << i;
      auto distance = prevValue - decayed;
      if (prevDistance) {
        ASSERT_LE(abs(prevDistance - distance), 1) << "distance increased unexpectedly i: " << i;
      }
      prevDistance = distance;
    }
    prevValue = decayed;
  }
}

TEST(GradidoUnitTest, TestReverseDecay)
{
  const auto startValue = GradidoUnit::fromGradidoCent(1000000);
  for (int i = 1; i < 31556952 * 2; i += 32) {
    auto valueWithDecay = startValue.calculateDecay(-i);
    auto decay = valueWithDecay.calculateDecay(i);
    ASSERT_LE(abs(startValue.getGradidoCent() - valueWithDecay.calculateDecay(i).getGradidoCent()), 1);
  }
}

TEST(GradidoUnitTest, With100Gdd14DaysDecay)
{
  auto startValue = GradidoUnit::fromGradidoCent(1000000);
  auto decayed = startValue.calculateDecay(60 * 60 * 24 * 14);
  // printf("decayed value: %s\n", decayed.toString().data());
  ASSERT_EQ(decayed.getGradidoCent(), 973781);
}

TEST(GradidoUnitTest, FromStringNegativeSubInteger)
{
  auto v = GradidoUnit::fromString("-0.0001");
  EXPECT_EQ(v.getGradidoCent(), -1);
}

TEST(GradidoUnitTest, toString_AllCases)
{
  // positive values
  GradidoUnit v(0.0);
  EXPECT_EQ("0", v.toString(0));
  EXPECT_EQ("0.0", v.toString(1));
  EXPECT_EQ("0.00", v.toString(2));
  EXPECT_EQ("0.000", v.toString(3));
  EXPECT_EQ("0.0000", v.toString(4));

  v = GradidoUnit(0.0001);
  EXPECT_EQ("0.0001", v.toString(4));
  EXPECT_EQ("0.000", v.toString(3));
  EXPECT_EQ("0.00", v.toString(2));
  EXPECT_EQ("0.0", v.toString(1));
  EXPECT_EQ("0", v.toString(0));

  v = GradidoUnit(0.001);
  EXPECT_EQ("0.001", v.toString(3));
  EXPECT_EQ("0.0010", v.toString(4));

  v = GradidoUnit(1.0);
  EXPECT_EQ("1", v.toString(0));
  EXPECT_EQ("1.0", v.toString(1));
  EXPECT_EQ("1.00", v.toString(2));
  EXPECT_EQ("1.000", v.toString(3));
  EXPECT_EQ("1.0000", v.toString(4));

  v = GradidoUnit(1234.5678);
  EXPECT_EQ("1235", v.toString(0));
  EXPECT_EQ("1234.6", v.toString(1));
  EXPECT_EQ("1234.57", v.toString(2));
  EXPECT_EQ("1234.568", v.toString(3));
  EXPECT_EQ("1234.5678", v.toString(4));

  // negative values
  v = GradidoUnit(-0.0);
  EXPECT_EQ("0", v.toString(0));

  v = GradidoUnit(-0.0001);
  EXPECT_EQ("-0.0001", v.toString(4));
  EXPECT_EQ("0.000", v.toString(3));
  EXPECT_EQ("0.00", v.toString(2));
  EXPECT_EQ("0.0", v.toString(1));
  EXPECT_EQ("0", v.toString(0));

  v = GradidoUnit(-1.0);
  EXPECT_EQ("-1", v.toString(0));
  EXPECT_EQ("-1.0", v.toString(1));
  EXPECT_EQ("-1.00", v.toString(2));
  EXPECT_EQ("-1.000", v.toString(3));
  EXPECT_EQ("-1.0000", v.toString(4));

  v = GradidoUnit(-1234.5678);
  EXPECT_EQ("-1235", v.toString(0));
  EXPECT_EQ("-1234.6", v.toString(1));
  EXPECT_EQ("-1234.57", v.toString(2));
  EXPECT_EQ("-1234.568", v.toString(3));
  EXPECT_EQ("-1234.5678", v.toString(4));

  // precision overflow, auto capped to 4
  v = GradidoUnit(1.2345678);
  EXPECT_EQ("1.2346", v.toString(5));
  EXPECT_EQ("1.2346", v.toString(10));

  // check if round work as expected
  v = GradidoUnit(0.00005);
  EXPECT_EQ("0.0001", v.toString(4));
  v = GradidoUnit(0.00004);
  EXPECT_EQ("0.0000", v.toString(4));

  v = GradidoUnit::fromGradidoCent(3000);
  EXPECT_EQ("0.3", v.toString(1));
  EXPECT_EQ("0.30", v.toString(2));
  EXPECT_EQ("0.300", v.toString(3));
  EXPECT_EQ("0.3000", v.toString(4));

  v = GradidoUnit::fromGradidoCent(300);
  EXPECT_EQ("0.0", v.toString(1));
  EXPECT_EQ("0.03", v.toString(2));
  EXPECT_EQ("0.030", v.toString(3));
  EXPECT_EQ("0.0300", v.toString(4));

  v = GradidoUnit::fromGradidoCent(3070);
  EXPECT_EQ("0.3", v.toString(1));
  EXPECT_EQ("0.31", v.toString(2));
  EXPECT_EQ("0.307", v.toString(3));
  EXPECT_EQ("0.3070", v.toString(4));

  v = GradidoUnit::fromGradidoCent(9223372036854775807);
  // Calls fail if biggest value for int64: 9223372036854775807 is exceeded
  EXPECT_EQ(v.toString(), "922337203685477.5807");
  // this would round up to: 9223372036854775810 which exceeds int64
  EXPECT_THROW(v.toString(3), FixedPointedArithmetikOverflowException);
  EXPECT_EQ(v.toString(2), "922337203685477.58");
  // this would round up to: 9223372036854776000 which exceeds int64
  EXPECT_THROW(v.toString(1), FixedPointedArithmetikOverflowException);
  // this would round up to: 9223372036854780000 which exceeds int64
  EXPECT_THROW(v.toString(0), FixedPointedArithmetikOverflowException);

  v = GradidoUnit::fromGradidoCent(-9223372036854775807);
  // Calls fail if biggest value for int64: -9223372036854775807 is exceeded
  EXPECT_EQ(v.toString(), "-922337203685477.5807");
  // this would round up to: -9223372036854775810 which exceeds int64
  EXPECT_THROW(v.toString(3), FixedPointedArithmetikOverflowException);
  EXPECT_EQ(v.toString(2), "-922337203685477.58");
  // this would round up to: -9223372036854776000 which exceeds int64
  EXPECT_THROW(v.toString(1), FixedPointedArithmetikOverflowException);
  // this would round up to: -9223372036854780000 which exceeds int64
  EXPECT_THROW(v.toString(0), FixedPointedArithmetikOverflowException);

  // bigger number
  v = GradidoUnit(987654321.1234);
  EXPECT_EQ("987654321", v.toString(0));
  EXPECT_EQ("987654321.1", v.toString(1));
  EXPECT_EQ("987654321.12", v.toString(2));
  EXPECT_EQ("987654321.123", v.toString(3));
  EXPECT_EQ("987654321.1234", v.toString(4));

  // number outside of double range
}

TEST(GradidoUnitTest, toString_Randomized)
{
  std::mt19937_64 rng(42); // deterministisch
  std::uniform_real_distribution<double> dist(-1e9, 1e9); // innerhalb double-Bereich
  int countDiffBetweenDoubleInteger = 0;

  for (int i = 0; i < 100'000; ++i) {
    double value = dist(rng);
    GradidoUnit v(value);

    for (int precision = 0; precision <= 4; ++precision) {
      std::string fastStr = v.toString(precision);

      if (precision < 0 || precision > 4) {
        throw GradidoNodeInvalidDataException("expect precision in the range [0;4]");
      }
      std::stringstream ss;
      ss << std::fixed << std::setprecision(precision);
      double rounded = v;
      if (precision < 4) {
        // round down like nodejs
        double factor = pow(10.0, precision);
        rounded = round(v * factor) / factor;
      }
      ss << rounded;
      std::string refStr(ss.str());

      if (fastStr != refStr) {
        ++countDiffBetweenDoubleInteger;
      }
      //EXPECT_EQ(fastStr, refStr) << "value=" << value << " precision=" << precision << " i=" << i;
      ASSERT_EQ(fastStr, v.toString(precision)) << "value=" << value << " precision=" << precision << " i=" << i;
    }
  }
  std::cout << COUT_GTEST_BLU << "double rounding errors: " << countDiffBetweenDoubleInteger << ANSI_TXT_DFT << std::endl;
}

TEST(GradidoUnitTest, toStringFast_RandomExact)
{
  std::mt19937_64 rng(42);

  std::uniform_int_distribution<int64_t> intDist(-1'000'000'000, 1'000'000'000);
  std::uniform_int_distribution<int> fracDist(0, 9999);

  for (int i = 0; i < 100'000; ++i) 
  {
    int64_t integerPart = intDist(rng);
    int fractionalPart = fracDist(rng);

    // Handle sign correctly
    bool negative = integerPart < 0;
    int64_t absInteger = std::abs(integerPart);

    // Build exact int64 value (GradidoCent)
    int64_t raw = absInteger * 10000 + fractionalPart;
    if (negative) raw = -raw;

    auto v = GradidoUnit::fromGradidoCent(raw);

    // Build exact string
    std::stringstream ss;
    if (negative) ss << "-";
    ss << absInteger << "."
      << std::setw(4) << std::setfill('0') << fractionalPart;

    std::string fullStr = ss.str();

    // --- toString Tests ---
    for (int precision = 0; precision <= 4; ++precision) {

      std::string str = v.toString(precision);

      std::string ref = fullStr;

      if (precision == 0) {
        int64_t roundedAbsInteger = absInteger;
        if (fractionalPart >= 5000) {
          ++roundedAbsInteger;
        }
        ref = (negative ? "-" : "") + std::to_string(roundedAbsInteger);
      }
      else {
        int64_t factor = pow(10.0, 4 - precision);
        int64_t rounded = (raw >= 0)
          ? (raw + factor / 2) / factor * factor
          : (raw - factor / 2) / factor * factor;

        int64_t integerPart = rounded / 10000;
        int64_t fractional = std::llabs(rounded % 10000);

        std::stringstream ss2;
        ss2 << integerPart;

        if (precision > 0) {
          ss2 << '.'
            << std::setw(precision)
            << std::setfill('0')
            << (fractional / factor);
        }
        ref = ss2.str();
      }

      ASSERT_EQ(str, ref)
        << "raw=" << raw
        << " precision=" << precision
        << " full=" << fullStr
        << " i=" << i;
    }

    // --- fromString Roundtrip ---
    auto parsed = GradidoUnit::fromString(fullStr);

    ASSERT_EQ(parsed.getGradidoCent(), raw)
      << "Parsing failed for " << fullStr;
  }
}

TEST(GradidoUnitTest, roundToPrecision_EdgeCases)
{
  struct TestCase {
    int64_t raw; // GradidoCent (4 decimal places)
    int precision;
    int64_t expected;
  };

  std::vector<TestCase> cases = {

    // --- simple cases ---
    {1234500, 2, 1234500},   // 123.4500 -> 123.45
    {1234567, 2, 1234600},   // 123.4567 -> 123.46
    {1234549, 2, 1234500},   // 123.4549 -> 123.45

    // --- critical .5 boundary ---
    {1000500, 1, 1001000},   // 100.0500 -> 100.050 -> round up
    {1000499, 1, 1000000},   // 100.0499 -> down

    // --- precision 0 ---
    {1234999, 0, 1230000},   // 123.4999 -> 123
    {1235000, 0, 1240000},   // 123.5000 -> 124

    // --- carry in integer ---
    {9999500, 0, 10000000},  // 999.9500 -> 1000
    {19999500, 1, 20000000}, // 1999.9500 -> 2000.0

    // --- negative values ---
    {-1234500, 2, -1234500}, // -123.4500 -> -123.45
    {-1234567, 2, -1234600}, // -123.4567 -> -123.46
    {-1234549, 2, -1234500}, // -123.4549 -> -123.45

    // --- critical negative .5 ---
    {-1235000, 0, -1240000}, // -123.5000 -> -124 (!! important !!)
    {-1234999, 0, -1230000}, // -123.4999 -> -123

    // --- small values ---
    {5, 4, 5},               // 0.0005 -> remains
    {5, 3, 10},               // 0.0005 -> 0.001
    {4, 3, 0},               // 0.0004 -> 0.000

    // --- null ---
    {0, 0, 0},
    {0, 4, 0},

    // --- extreme values ---
    {INT64_MAX / 10, 4, INT64_MAX / 10}, // only stability
    {INT64_MIN / 10, 4, INT64_MIN / 10}
  };

  for (const auto& t : cases) {
    auto v = GradidoUnit::fromGradidoCent(t.raw);
    auto rounded = v.roundToPrecision(t.precision);

    EXPECT_EQ(rounded.getGradidoCent(), t.expected)
      << "raw=" << t.raw
      << " precision=" << t.precision;
  }
}


TEST(GradidoUnitTest, roundToPrecision_BoundarySweep)
{
  for (int precision = 0; precision <= 4; ++precision) 
  {
    int64_t factor = pow(10.0, 4 - precision);

    for (int64_t base = -10; base <= 10; ++base) 
    {
      int64_t center = base * factor;

      // test around the 5. border
      for (int offset = -10; offset <= 10; ++offset) 
      {
        int64_t raw = center + offset;

        auto v = GradidoUnit::fromGradidoCent(raw);
        auto r = v.roundToPrecision(precision);

        int64_t expected =
          (raw >= 0)
          ? (raw + factor / 2) / factor * factor
          : (raw - factor / 2) / factor * factor;

        ASSERT_EQ(r.getGradidoCent(), expected)
          << "raw=" << raw
          << " precision=" << precision;
      }
    }
  }
}
