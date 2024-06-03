#include "gradido_blockchain/memory/MPFRBlock.h"
#include "loguru/loguru.hpp"

#ifdef USE_MPFR
namespace memory {

	std::mutex MPFRBlock::mMpfrMutex;
	std::stack<mpfr_ptr> MPFRBlock::mMpfrPtrStack;
	std::set<mpfr_ptr> MPFRBlock::mActiveMpfrs;

	MPFRBlock::MPFRBlock()
	{
		mpfr_ptr mathMemory;
		std::scoped_lock<std::mutex> _lock(mMpfrMutex);
		if (mMpfrPtrStack.size()) {
			mathMemory = mMpfrPtrStack.top();
			mMpfrPtrStack.pop();
		}
		else {
			mathMemory = new mpfr_t;
		}
		if (!mActiveMpfrs.insert(mathMemory).second) {
			assert(false && "[Manager::getMathMemory] try to return mathe memory already in use");
		}
		mpfr_init2(mathMemory, MAGIC_NUMBER_AMOUNT_PRECISION_BITS);
		mpfr_set_ui(mathMemory, 0, gDefaultRound);
		mData = mathMemory;
	}
	// move
	MPFRBlock::MPFRBlock(MPFRBlock&& src) noexcept
	{
		mData = src.mData;
		src.mData = nullptr;
	}

	MPFRBlock::~MPFRBlock()
	{
		if (!mData) return;
		std::scoped_lock<std::mutex> _lock(mMpfrMutex);

		if (mData->_mpfr_prec != MAGIC_NUMBER_AMOUNT_PRECISION_BITS) {
			LOG_F(ERROR, "wrong precision: %d", mData->_mpfr_prec);
			assert(false && "wrong precision");
		}
		mMpfrPtrStack.push(mData);
		if (!mActiveMpfrs.erase(mData)) {
			assert(false && "[Manager::getMathMemory] try to remove math memory already removed");
		}
	}

	std::unique_ptr<MPFRBlock> MPFRBlock::create()
	{
		return std::unique_ptr<MPFRBlock>(new MPFRBlock);
	}

	void MPFRBlock::clear()
	{
		std::scoped_lock<std::mutex> _lock(mMpfrMutex);
		while (mMpfrPtrStack.size() > 0) {
			mpfr_ptr mathMemory = mMpfrPtrStack.top();
			mMpfrPtrStack.pop();
			mpfr_clear(mathMemory);
			delete mathMemory;
		}
	}
}
#endif