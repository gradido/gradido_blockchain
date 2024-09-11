#include "gradido_blockchain/data/iota/MessageId.h"
#include "gradido_blockchain/data/iota/Exceptions.h"

#include <cassert>

namespace iota {
	MessageId::MessageId()
	{
		mMessageId.fill(0);
	}

	MessageId::MessageId(const memory::Block& messageId)
	{
		if (messageId.size() != sizeof(uint64_t) * mMessageId.size()) {
			throw InvalidSizeException("invalid input for message id", sizeof(uint64_t) * mMessageId.size(), messageId.size());
		}
		memcpy(mMessageId.data(), messageId, 4 * sizeof(uint64_t));
	}

	MessageId::MessageId(const std::string& messageIdHex)
		: MessageId(memory::Block::fromHex(messageIdHex))
	{

	}

	memory::Block MessageId::toMemoryBlock() const
	{
		return memory::Block(mMessageId.size() * sizeof(uint64_t), (unsigned char*)mMessageId.data());
	}
	std::string MessageId::toHex() const
	{
		const auto hexSize = 8 * sizeof(uint64_t) + 1;
		char hexTmp[hexSize];
		sodium_bin2hex(hexTmp, hexSize, reinterpret_cast<const unsigned char*>(mMessageId.data()), 4 * sizeof(uint64_t));
		return std::string(hexTmp, hexSize - 1);
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