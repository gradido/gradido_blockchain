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