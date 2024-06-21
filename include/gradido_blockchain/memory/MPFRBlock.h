#ifndef __GRADIDO_BLOCKCHAIN_MEMORY_MPFR_BLOCK_H
#define __GRADIDO_BLOCKCHAIN_MEMORY_MPFR_BLOCK_H

#include "gradido_blockchain/export.h"
#ifdef USE_MPFR
#include "gradido_blockchain/lib/Decay.h"

#include <memory>
#include <mutex>
#include <stack>
#include <set>

// MAGIC NUMBER: Define how many bits are used for amount calculation
#define MAGIC_NUMBER_AMOUNT_PRECISION_BITS 256

namespace memory {

	class GRADIDOBLOCKCHAIN_EXPORT MPFRBlock
	{
	public:
		MPFRBlock();
		~MPFRBlock();
		// copy constructor
		MPFRBlock(const MPFRBlock& src);
		MPFRBlock(MPFRBlock&& src) noexcept;

		static std::unique_ptr<MPFRBlock> create();

		mpfr_ptr data() { return mData; }
		const mpfr_ptr data() const { return mData; }
		inline operator mpfr_ptr() { return mData; }

	protected:		
		static void clear();

		mpfr_ptr mData;

		static std::mutex mMpfrMutex;
		static std::stack<mpfr_ptr> mMpfrPtrStack;
		static std::set<mpfr_ptr> mActiveMpfrs;
	};

}
#endif //USE_MPFR
#endif //__GRADIDO_BLOCKCHAIN_MEMORY_MPFR_BLOCK_H