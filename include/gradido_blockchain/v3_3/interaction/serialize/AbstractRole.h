#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"
#include "sodium.h"

#include <bit>
#include <array>
#include <vector>
#include "protopuf/message.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {

#define RUM_IMPLEMENTATION	memory::ConstBlockPtr run() const { return encode(getMessage());}

				class AbstractRole
				{
				public:
					virtual ~AbstractRole() {};
					virtual memory::ConstBlockPtr run() const = 0; 
					virtual size_t calculateSerializedSize() const = 0;

				protected:

					template <typename T>
					memory::ConstBlockPtr encode(T message) const 
					{
						auto size = calculateSerializedSize() + 10;
						std::vector<std::byte, memory::VectorCacheAllocator<std::byte>> buffer;
						buffer.resize(size, std::byte{ 0 });
						// auto buffer = std::make_shared<memory::Block>(size);
						auto bufferEnd = pp::message_coder<T>::encode(message, buffer);
						buffer.resize(pp::begin_diff(bufferEnd, buffer));
						//pp::bytes
						return std::make_shared<memory::Block>(buffer);
					}
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H