#include "gradido_blockchain/model/protopuf/VectorCacheAllocator.h"
#include "gradido_blockchain/lib/Profiler.h"
#include <gtest/gtest.h>

#include <vector>

using namespace model::protopuf;

// test that with using VectorCacheAllocator, vector is faster, after enough memory blocks are cached
TEST(VectorCacheAllocator, sizeAfterReservePublicKey) {
	Profiler timeUsed;
	{
		std::vector<uint8_t, VectorCacheAllocator<uint8_t>> testVector;
		for (int i = 0; i < 512; ++i) {
			testVector.push_back(rand());
		}
	}
	auto timeAfterFirst = timeUsed.nanos();
	auto mapSizeAfterFirstRun = VectorCacheAllocator<uint8_t>::getMapSize();
	timeUsed.reset();

	{
		std::vector<uint8_t, VectorCacheAllocator<uint8_t>> testVector;
		for (int i = 0; i < 512; ++i) {
			testVector.push_back(rand());
		}
	}
	EXPECT_LT(timeUsed.nanos(), timeAfterFirst);
	auto mapSizeAfterSecondRun = VectorCacheAllocator<uint8_t>::getMapSize();
	EXPECT_EQ(mapSizeAfterFirstRun, mapSizeAfterSecondRun);
}
