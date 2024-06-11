#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_TYPE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_TYPE_H

namespace gradido {
	namespace v3_3 {
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
          //! check also with previous transaction
          SINGLE_PREVIOUS = 2,
          /*! check all transaction from within date range
            by creation automatic the same month
          */
          DATE_RANGE = 4,
          //! check paired transaction on another group by cross group transactions
          PAIRED = 8,
          //! check all transactions in the group which connected with this transaction address(es)
          CONNECTED_GROUP = 16,
          //! check all transactions which connected with this transaction
          CONNECTED_BLOCKCHAIN = 32			
        };
      }
    }
  }
}

#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_TYPE_H