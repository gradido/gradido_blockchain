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



#include <thread>
#include <deque>

struct ThreadResult {
  int64_t exactMatches = 0;
  int64_t diffByOne = 0;
  int64_t diffByOther = 0;
  std::deque<std::tuple<int64_t, int64_t, int64_t>> errors; // amount, duration, diff
};

TEST(GradidoUnitTest, testManyCasesDecayRevertDecayRandom)
{
  constexpr int64_t NUM_SAMPLES = 5000000; // 500k test cases
  unsigned int NUM_THREADS = std::thread::hardware_concurrency();
  constexpr int64_t MAX_AMOUNT_CENT = 1'000'000ll * 1000ll; // 1M Gradido * 10000 Cent = 1e13 Cent
  constexpr int64_t MAX_DURATION_SECONDS = 60ll * 60ll * 24ll * 90ll; // 90 Days in seconds

  std::atomic<int64_t> totalTests{ 0 };
  std::atomic<int64_t> exactMatches{ 0 };
  std::atomic<int64_t> diffByOne{ 0 };
  std::atomic<int64_t> diffByOther{ 0 };
  std::atomic<int64_t> diffByExactTenThousandth{ 0 }; // 0.0001 corresponds to 1 Cent difference in the Cent range

  // Mutex only for outputting error examples, not for counting
  std::mutex coutMutex;
  std::vector<std::thread> threads;

  auto worker = [&](int threadId) {
    // Each thread gets its own random generator
    std::random_device rd;
    std::mt19937_64 gen(rd() + threadId);
    std::uniform_int_distribution<int64_t> amountDist(1, MAX_AMOUNT_CENT);
    std::uniform_int_distribution<int64_t> durationDist(1, MAX_DURATION_SECONDS);

    for (int64_t i = 0; i < NUM_SAMPLES / NUM_THREADS; ++i) {
      int64_t amountCent = amountDist(gen);
      int64_t duration = durationDist(gen);

      // Ignore duration == 0, since decay is then identity
      if (duration == 0 || amountCent == 0) continue;

      GradidoUnit original = GradidoUnit::fromGradidoCent(amountCent);
      auto amountWithBuff = original.calculateDecay(-duration);
      auto decayed = amountWithBuff.calculateDecay(duration);
      // GradidoUnit decayed = original.calculateDecay(duration);
      // GradidoUnit reversed = decayed.calculateDecay(-duration);

      // int64_t originalCent = original.getGradidoCent();
      // int64_t reversedCent = reversed.getGradidoCent();

      totalTests++;

      if (original == decayed) {
        exactMatches++;
      }
      else {
        GradidoUnit diff;
        if (original > decayed) {
          diff = original - decayed;
        }
        else {
          diff = decayed - original;
        }
        if (diff.getGradidoCent() == 1) {
          diffByOne++;
        }
        else {
          diffByOther++;
        }

        // Specifically check if the difference is 0.0001 (i.e., 1 Cent in the internal format)
        // But be careful: 0.0001 in Gradido units corresponds to 1 GradidoCent.
        // Since your previous error was "0.0001", you probably mean 1 Cent difference.
        // So: diff == 1 or -1. We already count this in diffByOne.

        // Optional: We log some examples
        if (diff != 0) {
          std::lock_guard<std::mutex> lock(coutMutex);
          // Only output every 1000th example, otherwise it becomes too much
          static int sampleCounter = 0;
          if (sampleCounter++ % 1000 == 0) {
            std::cout << "Amount: " << original.toString(4)
              << ", Duration: " << duration << "s"
              << ", amountWithBuff: " << amountWithBuff.toString(4)
              << ", decayed: " << decayed.toString(4)
              << ", Diff: " << diff.toString(4)
              << std::endl;
          }
        }
      }
    }
    };

  // Start threads
  for (unsigned int t = 0; t < NUM_THREADS; ++t) {
    threads.emplace_back(worker, t);
  }

  // Wait for all threads
  for (auto& th : threads) {
    th.join();
  }

  // Output results
  std::cout << "\n=== Sample Results (" << totalTests << " Tests) ===" << std::endl;
  std::cout << "Exact match: " << exactMatches << " ("
    << (100.0 * exactMatches / totalTests) << "%)" << std::endl;
  std::cout << "Difference by +-1 Cent (0.0001 GDD): " << diffByOne << " ("
    << (100.0 * diffByOne / totalTests) << "%)" << std::endl;
  std::cout << "Other difference: " << diffByOther << " ("
    << (100.0 * diffByOther / totalTests) << "%)" << std::endl;

  EXPECT_EQ(exactMatches, totalTests);
}


TEST(GradidoUnitTest, testManyCasesDecayRevertDecay)
{
  Profiler timeUsed;
  unsigned int NUM_THREADS = std::thread::hardware_concurrency();

  // We sample logarithmically across the value range to cover all orders of magnitude.
  // Because errors in floating point arithmetic often depend on the order of magnitude.
  std::deque<int64_t> amountSamples;
  std::deque<int64_t> durationSamples;

  // 1. Amounts: From 1 Cent to 10 billion Gradido, logarithmically distributed
  for (int64_t exp = 0; exp <= 14; ++exp) { // 10^0 = 1 Cent to 10^14 Cent = 10^10 GDD
    int64_t base = static_cast<int64_t>(std::pow(10.0, exp));
    for (int64_t mul = 1; mul <= 9; ++mul) {
      amountSamples.push_back(mul * base);
    }
  }
  // Also some odd values, near overflows
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 10000); // Maximum GDD value in Cent
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 2);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 3);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 4);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 5);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 10);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 100);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 500);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 1000);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 2000);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 5000);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 9000);
  amountSamples.push_back(std::numeric_limits<int64_t>::max() / 9900);
  amountSamples.push_back(70000000000001);
  amountSamples.push_back(69999999999999);
  for (int64_t i = 69000000000000; i < 69000000800001; i++) {
    amountSamples.push_back(i);
  }
  amountSamples.push_back(69000000000000);
  amountSamples.push_back(12345678901234LL); // Just because it's pretty

 

  constexpr int64_t SECONDS_PER_YEAR = 31556952; // 365.2425 days
  
  for (int64_t exp = 0; exp <= 6; ++exp) { // 10^0 = 1s to 10^6 seconds ~ 115 days
    int64_t base = static_cast<int64_t>(std::pow(10.0, exp));
    for (int64_t mul = 1; mul <= 9; ++mul) {
      durationSamples.push_back(mul * base);
    }
  }
  // Some typical durations
  durationSamples.push_back(60);                      // 1 minute
  durationSamples.push_back(3600);                    // 1 hour
  durationSamples.push_back(86400);                   // 1 day
  durationSamples.push_back(86400ll * 30ll);              // ~1 month
  durationSamples.push_back(86400ll * 60ll);              // ~2 months
  durationSamples.push_back(86400ll * 90ll);              // ~3 months
  
  durationSamples.push_back(86400 * 14);                   // 1 day

  // Remove duplicates and sort for a good feeling
  std::sort(amountSamples.begin(), amountSamples.end());
  amountSamples.erase(std::unique(amountSamples.begin(), amountSamples.end()), amountSamples.end());
  std::sort(durationSamples.begin(), durationSamples.end());
  durationSamples.erase(std::unique(durationSamples.begin(), durationSamples.end()), durationSamples.end());

  // Remove 0 from durations (makes no sense)
  durationSamples.erase(std::remove(durationSamples.begin(), durationSamples.end(), 0), durationSamples.end());

  size_t totalTests = amountSamples.size() * durationSamples.size();
  std::cout << "Testing " << amountSamples.size() << " amounts x " << durationSamples.size()
    << " durations = " << totalTests << " combinations." << std::endl;

  std::cout << "Time for preparations: " << timeUsed.string() << std::endl;

  std::vector<ThreadResult> threadResults(NUM_THREADS);
  std::vector<std::thread> threads;

  // Distribute the work evenly across threads
  size_t chunkSize = (totalTests + NUM_THREADS - 1) / NUM_THREADS;
  std::atomic<int64_t> smallestError = std::numeric_limits<int64_t>::max();
  std::atomic<int64_t> smallestErrorDuration;

  // 9223372036854775807
       // 7'000'000'000,0000

  for (unsigned int t = 0; t < NUM_THREADS; ++t)
  {
    threads.emplace_back([&, t]()
      {
        size_t startIdx = t * chunkSize;
        size_t endIdx = std::min(startIdx + chunkSize, totalTests);

        auto& res = threadResults[t];

        for (size_t idx = startIdx; idx < endIdx; ++idx) {
          // Calculate 2D index from flat index
          size_t amountIdx = idx / durationSamples.size();
          size_t durationIdx = idx % durationSamples.size();

          // Skip if index is out of bounds (should not happen)
          if (amountIdx >= amountSamples.size()) continue;

          int64_t amountCent = amountSamples[amountIdx];
          int64_t duration = durationSamples[durationIdx];

          GradidoUnit original = GradidoUnit::fromGradidoCent(amountCent);
          GradidoUnit buffed = original.calculateDecay(-duration);
          GradidoUnit decayed = buffed.calculateDecay(duration);

          int64_t diff;
          if (original > decayed) {
            diff = (original - decayed).getGradidoCent();
          }
          else {
            diff = (decayed - original).getGradidoCent();
          }
          if (diff >= 1) {
            if (amountCent < smallestError) {
              smallestError = amountCent;
              smallestErrorDuration = duration;
            }
          }
          if (diff < 1) {
            res.exactMatches++;
          }
          else if (diff == 1) {
            res.diffByOne++;
          }
          else {
            res.diffByOther++;

            // Store each error for later analysis (or limit the number)
            if (res.errors.size() < 1000) { // Do not store too many
              res.errors.emplace_back(amountCent, duration, diff);
            }
          }
        }
      }
    );
  }

  for (auto& th : threads) {
    th.join();
  }

  // Aggregate results
  int64_t totalExact = 0, totalDiffOne = 0, totalDiffOther = 0;
  std::vector<std::tuple<int64_t, int64_t, int64_t>> allErrors;

  for (const auto& res : threadResults) {
    totalExact += res.exactMatches;
    totalDiffOne += res.diffByOne;
    totalDiffOther += res.diffByOther;
    for (const auto& err : res.errors) {
      if (allErrors.size() < 1000) {
        allErrors.push_back(err);
      }
    }
  }

  // Output
  std::cout << "\n=== Results of uniform grid tests (" << totalTests << " combinations) ===" << std::endl;
  std::cout << "Exact matches: " << totalExact << " ("
    << (100.0 * totalExact / totalTests) << "%)" << std::endl;
  std::cout << "Difference +-1 Cent (0.0001 GDD): " << totalDiffOne << " ("
    << (100.0 * totalDiffOne / totalTests) << "%)" << std::endl;
  std::cout << "Other differences: " << totalDiffOther << " ("
    << (100.0 * totalDiffOther / totalTests) << "%)" << std::endl;
  std::cout << "Smallest Error: " << smallestError << ", " << smallestErrorDuration << " seconds " << std::endl;
  // found 2,3 days, 6'900'000'000,0005 GDD as smallest error by duration range of 1 seconds - 115 days
  // for 14 days: no error found


  // Show the first 20 errors as examples
  std::cout << "\nExample errors (amount in Cent, duration in seconds, difference in Cent):" << std::endl;
  std::set<int64_t> postedGdd;
  for (size_t i = 0; i < allErrors.size(); ++i) {
    const auto& [amt, dur, diff] = allErrors[i];
    if (!postedGdd.insert(amt).second) {
      continue;
    }
    std::cout << "  " << amt << " Cent (" << (amt / 10000.0) << " GDD) | "
      << dur << "s | Diff: " << diff << " Cent = " << (diff / 10000.0) << " GDD" << std::endl;
    if (postedGdd.size() > 20) {
      break;
    }
  }

  EXPECT_GE((100.0 * totalExact / totalTests), 99.0);
}

TEST(GradidoUnitTest, testPrecisionDifferentTimeTransactions)
{
  using namespace std::chrono;

  // --- Define time points ---
  Timepoint start = Timepoint(seconds(0));
  Timepoint t2 = Timepoint(seconds(60 * 60 * 24 * 30));   // +30 days
  Timepoint t3 = Timepoint(seconds(60 * 60 * 24 * 90));   // +90 days
  Timepoint end = Timepoint(seconds(60 * 60 * 24 * 365));  // +1 year

  // --- Large values near int64 limit ---
  // int64 max ~9.22e18 -> we stay a bit below because of *10000
  int64_t maxSafeCent = std::numeric_limits<int64_t>::max() / 2;

  GradidoUnit startAmount = GradidoUnit::fromGradidoCent(maxSafeCent);
  GradidoUnit minusAmount = GradidoUnit::fromGradidoCent(100 * 10000); // -100 GDD
  GradidoUnit plusAmount = GradidoUnit::fromGradidoCent(500 * 10000); // +500 GDD

  // --- Variant 1: Step-by-step simulation ---
  GradidoUnit step = startAmount;

  // decay to t2
  step = step.calculateDecay(start, t2);

  // -100 GDD at t2
  step -= minusAmount;

  // decay to t3
  step = step.calculateDecay(t2, t3);

  // +500 GDD at t3
  step += plusAmount;

  // decay to end
  step = step.calculateDecay(t3, end);


  // --- Variant 2: Reference calculation ---
  GradidoUnit ref = startAmount.calculateDecay(start, end);

  // -100 GDD -> from t2 to end decayed
  GradidoUnit minusDecayed = minusAmount.calculateDecay(t2, end);
  ref -= minusDecayed;

  // +500 GDD -> from t3 to end decayed
  GradidoUnit plusDecayed = plusAmount.calculateDecay(t3, end);
  ref += plusDecayed;


  // --- Comparison ---
  // Not exact due to rounding -> Tolerance via GradidoCent
  int64_t stepCent = step.getGradidoCent();
  int64_t refCent = ref.getGradidoCent();

  int64_t diff = std::llabs(stepCent - refCent);

  // Tolerance: 1 Cent (0.0001 GDD)
  EXPECT_LE(diff, 1)
    << "Mismatch between step-by-step and reference calculation. "
    << "step=" << stepCent << " ref=" << refCent << " diff=" << diff;
}

TEST(GradidoUnitTest, testOverflowProvocation)
{
  using namespace std::chrono;

  // --- Time ---
  Timepoint start = Timepoint(seconds(0));

  // Negative time period -> Compound Interest (GROWTH)
  Duration hugeNegativeDuration = seconds(-60 * 60 * 24 * 365 * 10); // -10 years

  // --- Start value near limit ---
  int64_t nearMax = std::numeric_limits<int64_t>::max() / 10000 - 1;
  GradidoUnit value = GradidoUnit::fromGradidoCent(nearMax);

  bool overflowDetected = false;

  try {
    // Apply multiple times -> escalating growth
    for (int i = 0; i < 10; ++i) {
      value = value.calculateCompoundInterest(hugeNegativeDuration);

      // Enforce critical multiplication
      volatile int64_t cent = value.getGradidoCent();
      (void)cent;
    }
  }
  catch (const FixedPointedArithmetikOverflowException& ex) {
    overflowDetected = true;
    std::cout << "Expected overflow caught: " << ex.getFullString() << std::endl;
  }
  catch (...) {
    overflowDetected = true;
    std::cout << "Unknown overflow/exception caught" << std::endl;
  }

  EXPECT_FALSE(overflowDetected) << "Not expected overflow occur!";
}

TEST(GradidoUnitTest, findMaxSafeGradidoCent)
{
  int64_t low = 0;
  int64_t high = std::numeric_limits<int64_t>::max();

  int64_t lastGood = 0;

  while (low <= high && low != std::numeric_limits<int64_t>::max() && low != std::numeric_limits<int64_t>::min()) {
    int64_t mid = low + (high - low) / 2;
    
    GradidoUnit a = GradidoUnit::fromGradidoCent(mid);
    int64_t cent = a.getGradidoCent();
    GradidoUnit b = GradidoUnit::fromGradidoCent(cent);

    if (a == b) {
      lastGood = mid;
      low = mid + 1; // go even higher
    }
    else {
      high = mid - 1; // too large -> back
    }
  }

  std::cout << "Max safe GradidoCent: " << lastGood << std::endl;

  // Optional: directly show the first broken value
  GradidoUnit brokenA = GradidoUnit::fromGradidoCent(lastGood + 1);
  int64_t brokenCent = brokenA.getGradidoCent();
  GradidoUnit brokenB = GradidoUnit::fromGradidoCent(brokenCent);

  std::cout << "First broken value: " << (lastGood + 1) << std::endl;
  std::cout << "Roundtrip result: " << brokenCent << std::endl;

  EXPECT_TRUE(lastGood > 0);
}