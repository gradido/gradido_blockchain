#ifndef __GRADIDO_BLOCKCHAIN_DATA_MESSAGE_ID_H
#define __GRADIDO_BLOCKCHAIN_DATA_MESSAGE_ID_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/memory/Block.h"

#include <array>

namespace iota
{
	class GRADIDOBLOCKCHAIN_EXPORT MessageId
	{
	public:
		MessageId();
		//! \param messageId binary version of message id, 32 Bytes expected
		MessageId(const memory::Block& messageId);
		//! \param messageIdHex hex version of message id, 64 character expected
		MessageId(const std::string& messageIdHex);

		operator std::string() const { return toHex(); }

		//! operator needed for MessageId as key in unordered map
		bool operator==(const MessageId& other) const {
			// I don't use if or for to hopefully speed up the comparisation
			// maybe it can be speed up further with SSE, SSE2 or MMX (only x86 or amd64 not arm or arm64)
			// http://ithare.com/infographics-operation-costs-in-cpu-clock-cycles/
			// https://streamhpc.com/blog/2012-07-16/how-expensive-is-an-operation-on-a-cpu/
			return
				mMessageId[0] == other.mMessageId[0] &&
				mMessageId[1] == other.mMessageId[1] &&
				mMessageId[2] == other.mMessageId[2] &&
				mMessageId[3] == other.mMessageId[3];
		}

		// overload `<` operator to use a `MessageId` object as a key in a `std::map`
		// It returns true if the current object appears before the specified object
		bool operator<(const MessageId& ob) const {
			return
				mMessageId[0] < ob.mMessageId[0] ||
				(mMessageId[0] == ob.mMessageId[0] && mMessageId[1] < ob.mMessageId[1]) || (
					mMessageId[0] == ob.mMessageId[0] &&
					mMessageId[1] == ob.mMessageId[1] &&
					mMessageId[2] < ob.mMessageId[2]
					) || (
						mMessageId[0] == ob.mMessageId[0] &&
						mMessageId[1] == ob.mMessageId[1] &&
						mMessageId[2] == ob.mMessageId[2] &&
						mMessageId[3] < ob.mMessageId[3]
						);
		}		
		memory::Block toMemoryBlock() const;
		std::string toHex() const;
		bool isEmpty() const;

		inline uint64_t getMessageIdByte(uint8_t index) const { assert(index < 4); return mMessageId[index]; };

	protected:
		std::array<uint64_t, 4> mMessageId;
	};
}

// hashing function for message id
// simply use from first 8 Bytes up to size_t size
// should be super fast and enough different for unordered map hashlist because message ids are already hashes
namespace std {
	template <>
	struct hash<iota::MessageId> {
		size_t operator()(const iota::MessageId& k) const {
			return static_cast<size_t>(k.getMessageIdByte(0));
		}
	};
}

#endif //__GRADIDO_BLOCKCHAIN_DATA_MESSAGE_ID_H