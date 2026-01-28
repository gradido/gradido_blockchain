#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_EXCEPTIONS_H

#include "gradido_blockchain/GradidoBlockchainException.h"

namespace gradido {
	namespace interaction {
		namespace deserialize {
			class GRADIDOBLOCKCHAIN_EXPORT MissingMemberException : public GradidoBlockchainException
			{
			public:
				explicit MissingMemberException(const char* what, const char* memberName) noexcept
					: GradidoBlockchainException(what), mMemberName(memberName) {}
				std::string getFullString() const;

			protected:
				std::string mMemberName;

			};

			class GRADIDOBLOCKCHAIN_EXPORT EmptyMemberException : public GradidoBlockchainException
			{
			public:
				explicit EmptyMemberException(const char* what, const char* memberName) noexcept
					: GradidoBlockchainException(what), mMemberName(memberName) {
				}
				std::string getFullString() const;

			protected:
				std::string mMemberName;

			};
		}
	}
}


#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_DESERIALIZE_EXCEPTIONS_H