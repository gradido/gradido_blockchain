#ifndef __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_TRANSACTION_TYPE_H

#include <stdint.h>

namespace gradido {
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
			NONE,
			//! Creation Transaction, creates new Gradidos
			CREATION,
			//! Transfer Transaction, move Gradidos from one account to another
			TRANSFER,
			//! Group Friends Update Transaction, update relationship between groups
			COMMUNITY_FRIENDS_UPDATE,
			//! Register new address or sub address to group or move addres to another group
			REGISTER_ADDRESS,
			//! Special Transfer Transaction with timeout used for Gradido Link
			DEFERRED_TRANSFER,
			//! First Transaction in Blockchain
			COMMUNITY_ROOT,
			//! redeeming deferred transfer
			REDEEM_DEFERRED_TRANSFER
		};
	}
}

#endif //__GRADIDO_BLOCKCHAIN_v3_3_DATA_TRANSACTION_TYPE_H