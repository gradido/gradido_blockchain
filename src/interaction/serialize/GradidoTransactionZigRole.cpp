#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/serialize/GradidoTransactionZigRole.h"
#include "gradido_blockchain/memory/Block.h"

#include <memory>

using memory::Block, memory::ConstBlockPtr;
using std::make_shared;

namespace gradido {
	namespace interaction {
		namespace serialize {

			GradidoTransactionZigRole::GradidoTransactionZigRole(const data::GradidoTransaction& gradidoTransaction)
				: mGradidoTransaction(gradidoTransaction)
			{
			}

			ConstBlockPtr GradidoTransactionZigRole::run() const
			{
				uint8_t staticResultBuffer[1024];
				grdw_gradido_transaction tx;
				auto blockchainCommunityIdIndex = mGradidoTransaction.getCommunityIdIndex();
				mGradidoTransaction.toGrdw(&tx, blockchainCommunityIdIndex);
				auto encodeResult = grdw_gradido_transaction_encode(&tx, staticResultBuffer, 1024);
				grdw_gradido_transaction_free_deep(&tx);
				return make_shared<const Block>(encodeResult.written, staticResultBuffer);
			}

			size_t GradidoTransactionZigRole::calculateSerializedSize() const
			{
				return 0;
			}

		}
	}
}

