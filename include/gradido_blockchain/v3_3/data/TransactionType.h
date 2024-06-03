#ifndef __GRADIDO_BLOCKCHAIN_v3_3_DATA_TRANSACTION_TYPE_H
#define __GRADIDO_BLOCKCHAIN_v3_3_DATA_TRANSACTION_TYPE_H

#include <stdint.h>

namespace gradido {
	namespace v3_3 {
		namespace data {
		/*!
		 *  \addtogroup enums
		 *  @{
		 */
		 /*! @enum model::gradido::TransactionType
			 enum for different transaction types
		  */
		  /// Transaction Type Enum
			enum class TransactionType : uint8_t {
				//! Invalid or Empty Transaction
				TRANSACTION_NONE,
				//! Creation Transaction, creates new Gradidos
				TRANSACTION_CREATION,
				//! Transfer Transaction, move Gradidos from one account to another
				TRANSACTION_TRANSFER,
				//! Group Friends Update Transaction, update relationship between groups
				TRANSACTION_GROUP_FRIENDS_UPDATE,
				//! Register new address or sub address to group or move addres to another group
				TRANSACTION_REGISTER_ADDRESS,
				//! Special Transfer Transaction with timeout used for Gradido Link
				TRANSACTION_DEFERRED_TRANSFER,
				//! First Transaction in Blockchain
				TRANSACTION_COMMUNITY_ROOT
			};
		}
	}
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_DATA_TRANSACTION_TYPE_H