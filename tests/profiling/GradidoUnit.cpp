#include "gtest/gtest.h"
#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/lib/MonotonicTimer.h"
#include <random>


TEST(GradidoUnit, toString) {
  GradidoUnit value(1.2345);
  MonotonicTimer timeUsed;
  for (int i = 0; i < 1'000'000; i++) {
    value.toString();
  }
  printf("1 million times in: %s\n", timeUsed.string().c_str());
  int zahl = 8;
}

TEST(GradidoUnit, toStringCStyle) {
  GradidoUnit value(1.2345);
  MonotonicTimer timeUsed;
  char buffer[64];
  for (int i = 0; i < 1'000'000; i++) {
    value.toString(buffer, 64);
  }
  printf("1 million times in: %s\n", timeUsed.string().c_str());
  int zahl = 8;
}

TEST(GradidoUnit, fromString) {
  std::string stringValue("1.2345");
  MonotonicTimer timeUsed;
  for (int i = 0; i < 1'000'000; i++) {
    GradidoUnit::fromString(stringValue);
  }
  printf("1 million times in: %s\n", timeUsed.string().c_str());
  int zahl = 8;
}