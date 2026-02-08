#include "gradido_blockchain/data/GradidoTransaction.h"
#include "gradido_blockchain/interaction/serialize/GradidoTransactionZigRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"
#include "gradido_protobuf_zig.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr, memory::GrduStaticBuffer;
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
				GrduStaticBuffer<1024> staticInputBuffer;
				return staticInputBuffer.use(
					[&](grdu_memory* alloc) -> ConstBlockPtr
					{
						grdw_gradido_transaction tx{};
						auto blockchainCommunityIdIndex = mGradidoTransaction.getCommunityIdIndex();
						mGradidoTransaction.toGrdw(alloc, &tx, blockchainCommunityIdIndex);

						uint8_t staticResultBuffer[1024];
						auto encodeResult = grdw_gradido_transaction_encode(&tx, staticResultBuffer, 1024);
						if (GRDW_ENCODING_ERROR_OUT_OF_MEMORY == encodeResult.state) 
						{
							Block resultBuffer(2048);
							encodeResult = grdw_gradido_transaction_encode(&tx, resultBuffer, 2048);
							if (GRDW_ENCODING_ERROR_SUCCESS == encodeResult.state) {
								LOG_F(WARNING, "static output buffer was to small, used 2048 Bytes buffer, acutally used: %d Bytes", encodeResult.written);
								return make_shared<const Block>(encodeResult.written, resultBuffer);
							}
						}
						if (GRDW_ENCODING_ERROR_SUCCESS != encodeResult.state) 
						{
							LOG_F(ERROR, "encode error: %s", enum_name(encodeResult.state).data());
							throw GradidoNodeInvalidDataException("error serialize gradido transaction");
						}
						return make_shared<const Block>(encodeResult.written, staticResultBuffer);
					}
				);
			}

			size_t GradidoTransactionZigRole::calculateSerializedSize() const
			{
				return 0;
			}

		}
	}
}

