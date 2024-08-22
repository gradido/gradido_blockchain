#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H

#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"

#include <bit>
#include <array>
#include <vector>
#include "protopuf/message.h"
#include "sodium.h"

namespace gradido {
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
					if (!buffer.size()) return nullptr;
					//pp::bytes
					return std::make_shared<memory::Block>(buffer);
				}

				std::string GradidoUnitToStringTrimTrailingZeros(const GradidoUnit& GradidoUnit) const
				{
					// Trim trailing zeros
					auto str = GradidoUnit.toString();
					str.erase(str.find_last_not_of('0') + 1, std::string::npos);
					if (str.back() == '.') str.pop_back(); // Remove trailing GradidoUnit point if present

					return str;
				}
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H