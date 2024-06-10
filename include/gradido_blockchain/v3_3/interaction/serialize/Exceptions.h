#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_EXCEPTIONS_H

#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace serialize {
				class GRADIDOBLOCKCHAIN_EXPORT MissingMemberException : public GradidoBlockchainException
				{
				public:
					explicit MissingMemberException(const char* what, const char* memberName) noexcept
						: GradidoBlockchainException(what), mMemberName(memberName) {}
					std::string getFullString() const;

				protected:
					std::string mMemberName;

				};

			}
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_SERIALIZE_EXCEPTIONS_H