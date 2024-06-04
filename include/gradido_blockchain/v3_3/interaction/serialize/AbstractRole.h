#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H

#include "gradido_blockchain/v3_3/data/Protocol.h"
#include "Type.h"

#include <bit>
#include "protopuf/message.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {
				class AbstractRole
				{
				public:
					virtual ~AbstractRole() {};
					virtual memory::ConstBlockPtr run() const = 0;

					template <typename T>
					memory::ConstBlockPtr encode(T message, size_t size) const {
						auto buffer = std::make_shared<memory::Block>(size);
						//pp::message_coder<T>::encode(message, pp::bytes(*buffer, size));
						return buffer;
					}
				};
			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_ABSTRACT_ROLE_H