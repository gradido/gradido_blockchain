#include "gradido_blockchain_core/data/wire/gradido_transaction.h"
#include "arnm/memory_block.h"
#include "gradido_blockchain_core/result.h"
#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/serialize/GradidoTransactionRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <memory>

using namespace magic_enum;
using memory::Block, memory::BlockPtr, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::make_shared;

namespace gradido {
	namespace interaction {
		namespace serialize {

			GradidoTransactionRole::GradidoTransactionRole(const data::GradidoTransaction& gradidoTransaction)
				: mGradidoTransaction(gradidoTransaction)
			{
			}

			ConstBlockPtr GradidoTransactionRole::run() const
			{
				GrduStaticBuffer<2048> staticInputBuffer;
				BlockPtr resultPtr = nullptr;
				staticInputBuffer.use(
					[&](arnm* alloc) -> arnm_result
					{
						grdw_gradido_transaction tx{};
						auto blockchainCommunityIdIndex = mGradidoTransaction.getCommunityIdIndex();
						mGradidoTransaction.toGrdw(alloc, &tx, blockchainCommunityIdIndex);

						uint8_t staticResultBuffer[2048];
						int finalSize = 0;
						arnm_memory_block resultBuffer = { .data = staticResultBuffer, .size = 2048 };
						auto encodeResult = grdw_gradido_transaction_encode(&resultBuffer, &finalSize, &tx, alloc);
						if (ARNM_ERROR_DESTINATION_BUFFER_TO_SMALL == encodeResult)
						{
							Block dynamicResultBuffer(4096);
							resultBuffer.data = dynamicResultBuffer.data();
							resultBuffer.size = static_cast<uint32_t>(dynamicResultBuffer.size());
							encodeResult = grdw_gradido_transaction_encode(&resultBuffer, &finalSize, &tx, alloc);
							if (ARNM_SUCCESS == encodeResult) {
								LOG_F(WARNING, "static output buffer was to small, used 2048 Bytes buffer, acutally used: %d Bytes", finalSize);
								resultPtr = make_shared<Block>(finalSize, dynamicResultBuffer);
								return ARNM_SUCCESS;
							}
						}
						if (ARNM_ERROR_OUT_OF_MEMORY == encodeResult) {
							return encodeResult;
						}
						if (ARNM_SUCCESS != encodeResult)
						{
							LOG_F(ERROR, "encode error: %s", grd_result_to_string(encodeResult));
							throw GradidoNodeInvalidDataException("error serialize gradido transaction");
						}
						resultPtr = make_shared<Block>(finalSize, staticResultBuffer);
						return ARNM_SUCCESS;
					}
				);
				return resultPtr;
			}

			size_t GradidoTransactionRole::calculateSerializedSize() const
			{
				return 0;
			}

		}
	}
}

