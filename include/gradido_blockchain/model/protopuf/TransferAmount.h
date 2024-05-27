#ifndef __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TRANSFER_AMOUNT_H
#define __GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TRANSFER_AMOUNT_H

#include "protopuf/message.h"

namespace model {
	namespace protopuf {
        using TransferAmountMessage = pp::message<
            pp::bytes_field<"pubkey", 1>,
            pp::string_field<"amount", 2>,
            pp::string_field<"community_id", 3>
        >;

        class TransferAmount
        {
        public:
        protected:

        };
	}
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_PROTOPUF_TRANSFER_AMOUNT_H