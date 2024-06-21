#include "gradido_blockchain/data/iota/MessageId.h"
#include "gradido_blockchain/data/iota/Exceptions.h"

#include <cassert>

namespace iota {
	MessageId::MessageId() 
	{
		mMessageId.fill(0);
	}

	MessageId MessageId::fromMemoryBlock(const memory::Block& bin)
	{
		if (bin.size() != 4 * sizeof(uint64_t)) {
			throw MessageIdFormatException("message id as bin has wrong size", bin);
		}
		MessageId messageId;
		memcpy(messageId.mMessageId.data(), bin, 4 * sizeof(uint64_t));
		return messageId;
	}

	memory::Block MessageId::toMemoryBin() const
	{
		return memory::Block(mMessageId.size() * sizeof(uint64_t), (unsigned char*)mMessageId.data());
	}

	bool MessageId::isEmpty() const
	{
		return
			mMessageId[0] == 0 &&
			mMessageId[1] == 0 &&
			mMessageId[2] == 0 &&
			mMessageId[3] == 0;
	}
}