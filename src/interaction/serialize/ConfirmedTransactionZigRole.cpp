#include "gradido_blockchain/AppContext.h"
#include "gradido_blockchain/const.h"
#include "gradido_blockchain/data/ConfirmedTransaction.h"
#include "gradido_blockchain/interaction/serialize/ConfirmedTransactionZigRole.h"
#include "gradido_blockchain/interaction/serialize/Exceptions.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_protobuf_zig.h"

#include <memory.h>

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

namespace gradido {
	namespace interaction {
		namespace serialize {

			ConfirmedTransactionZigRole::ConfirmedTransactionZigRole(const data::ConfirmedTransaction& confirmedTransaction)
				: mConfirmedTransaction(confirmedTransaction)
			{
			}

			ConfirmedTransactionZigRole::~ConfirmedTransactionZigRole()
			{
			}

			ConstBlockPtr ConfirmedTransactionZigRole::run() const
			{
				uint8_t staticResultBuffer[1024];
				grdw_confirmed_transaction tx;
				auto blockchainCommunityIdIndex = mConfirmedTransaction.getGradidoTransaction()->getCommunityIdIndex();
				mConfirmedTransaction.toGrdw(&tx, blockchainCommunityIdIndex);
				auto encodeResult = grdw_confirmed_transaction_encode(&tx, staticResultBuffer, 1024);
				grdw_confirmed_transaction_free_deep(&tx);
				return make_shared<const Block>(encodeResult.written, staticResultBuffer);
			}


			size_t ConfirmedTransactionZigRole::calculateSerializedSize() const
			{
				return 0;
			}

		}
	}
}
