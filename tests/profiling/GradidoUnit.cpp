#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/Profiler.h"
#include <random>


TEST(GradidoUnit, toString) {
  GradidoUnit value(1.2345);
  Profiler timeUsed;
  for (int i = 0; i < 1'000'000; i++) {
    value.toString();
  }
  printf("time: %s\n", timeUsed.string().c_str());
  int zahl = 8;
}

TEST(GradidoUnit, fromString) {
  std::string stringValue("1.2345");
  Profiler timeUsed;
  for (int i = 0; i < 1'000'000; i++) {
    GradidoUnit::fromString(stringValue);
  }
  printf("time: %s\n", timeUsed.string().c_str());
  int zahl = 8;
}
constexpr long double SECONDS_PER_YEAR = 31556952.0;

TEST(GradidoUnit, drift)
{
  const double base = pow(2.0, -2.0 / SECONDS_PER_YEAR);   // 2^{-1/T}
  // const double exactBase = pow(2.0, -1.0 / SECONDS_PER_YEAR); // identisch, nur zur Klarheit

  std::cout << std::fixed << std::setprecision(15);
  std::cout << "Analyse des Drifts fuer Zweierpotenzen bis x/T = 1\n";
  std::cout << "------------------------------------------------\n";

  for (int n = 0; ; ++n) {
    long long x = 1LL << n;          // x = 2^n
    if (x > SECONDS_PER_YEAR) break;

    double direct = pow(2.0, -double(x) / SECONDS_PER_YEAR);
    double iterative = pow(base, double(x/2.0));
    double absDiff = std::abs(direct - iterative);
    double relDiff = absDiff / direct;

    std::cout << "x (" << n << ") = " << std::setw(12) << n << x
      << "  | direct = " << direct
      << "  | iter  = " << iterative
      << "  | abs drift = " << std::scientific << absDiff
      << "  | rel drift = " << relDiff << std::endl;
  }
  double gdd = 1000000;
  auto lastFactor = pow(base, double(1LL << 24));
  auto lastFactorR = pow(2.0, -double(1LL << 25) / SECONDS_PER_YEAR);
  auto gddDecayedStep = gdd * pow(base, double(1LL << 24));
  std::cout << gdd << " decayed: " << gddDecayedStep << std::fixed << std::endl;
  std::cout << lastFactor << ", R: " << lastFactorR << std::endl;
}