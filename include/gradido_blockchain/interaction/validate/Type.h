#ifndef __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TYPE_H
#define __GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TYPE_H

#ifndef SWIG
#include "magic_enum/magic_enum_flags.hpp"
#endif

namespace gradido {
	namespace interaction {
        namespace validate {

            /*! @enum model::gradido::TransactionValidationLevel
                enum used as Bitmask for different transaction validation levels
                so it is possible to validate with different levels at once.
                The main difference between the levels are the needed data for that and the complexity
            */
            enum class Type {
                //! check only the transaction
                SINGLE = 1,
                //! check with previous transaction
                //! previous transaction belonging to same address
                //! sender by transfer transaction
                PREVIOUS = 2,
                //! check all transaction from same address in same month
                //! by creation the target date month
                MONTH_RANGE = 4,
                //! check paired transaction on another group by cross group transactions
                PAIRED = 8,
                //! check if transaction is valid for account
                //! check if account exist on blockchain
                //! check sender and recipient account, at least remote access needed for cross group transactions
                ACCOUNT = 16,
                //! check all transactions in the group which connected with this transaction address(es)
                CONNECTED_GROUP = 32,
                //! check all transactions which connected with this transaction
                CONNECTED_BLOCKCHAIN = 64
            };

            constexpr inline Type operator | (Type lhs, Type rhs)
            {
                using T = std::underlying_type_t <Type>;
                return static_cast<Type>(static_cast<T>(lhs) | static_cast<T>(rhs));
            }

            constexpr inline Type operator & (Type lhs, Type rhs)
            {
                using T = std::underlying_type_t <Type>;
                return static_cast<Type>(static_cast<T>(lhs) & static_cast<T>(rhs));
            }

            constexpr inline Type operator - (Type lhs, Type rhs)
            {
                using T = std::underlying_type_t <Type>;
                return static_cast<Type>(static_cast<T>(lhs) - static_cast<T>(rhs));
            }

            constexpr bool hasFlag(Type v, Type f) {
                using T = std::underlying_type_t<Type>;
                return (static_cast<T>(v) & static_cast<T>(f)) != 0;
            }
        }
    }
}
#ifndef SWIG
template <>
struct magic_enum::customize::enum_range<gradido::interaction::validate::Type> {
	static constexpr bool is_flags = true;
};
#endif

#endif //__GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_TYPE_H