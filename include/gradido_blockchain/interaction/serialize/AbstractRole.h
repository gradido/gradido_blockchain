#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H

#include "gradido_blockchain/GradidoUnit.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/VectorCacheAllocator.h"

#include <bit>
#include <array>
#include <vector>
#include <string>
#include <optional>
#include "protopuf/message.h"
#include "sodium.h"

namespace gradido {
	namespace interaction {
		namespace serialize {

#define RUM_IMPLEMENTATION	memory::ConstBlockPtr run() const { return encode(getMessage());}

			class AbstractRole
			{
			public:
				AbstractRole() : mBlockchainCommunityIdIndex(0) {}
				virtual ~AbstractRole() {};
				virtual memory::ConstBlockPtr run() const = 0; 
				virtual size_t calculateSerializedSize() const = 0;
				inline void setBlockchainCommunityIdIndex(uint32_t blockchainCommunityIdIndex) {
					mBlockchainCommunityIdIndex = blockchainCommunityIdIndex;
				}

			protected:

				std::optional<uint32_t> mBlockchainCommunityIdIndex;

				template <typename T>
				memory::ConstBlockPtr encode(T message) const 
				{
					auto size = calculateSerializedSize() + 10;
					std::vector<std::byte, memory::VectorCacheAllocator<std::byte>> buffer;
					buffer.resize(size, std::byte{ 0 });
					// auto buffer = std::make_shared<memory::Block>(size);
					auto bufferEnd = pp::message_coder<T>::encode(message, buffer);
					if (!bufferEnd.has_value()) return nullptr;
					auto bufferEndValue = bufferEnd.value();
					buffer.resize(pp::begin_diff(bufferEndValue, buffer));
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

				// TODO: check exact values in protobuf manuel and in protopuf implementation
				static size_t serializedSize(uint64_t ui64Value) {
					if (ui64Value < 128) {
						return 1;
					}
					else if (ui64Value < 16000) {
						return 2;
					}
					else if (ui64Value < 2000000) {
						return 3;
					}
					else if (ui64Value < pow(2, 35)) {
						return 5;
					}
					return 10;
				}
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H