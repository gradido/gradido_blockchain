#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/serialize/TransactionBodyZigRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"
#include "gradido_protobuf_zig.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <memory>

using namespace magic_enum;
using memory::Block, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::make_shared;

namespace gradido::interaction::serialize {
	/*
			* GrduStaticBuffer<1024> staticInputBuffer;
				return staticInputBuffer.use([&](grdu_memory* alloc) -> ConstBlockPtr
				{
						grdw_confirmed_transaction tx{};
						auto blockchainCommunityIdIndex = mConfirmedTransaction.getGradidoTransaction()->getCommunityIdIndex();
						mConfirmedTransaction.toGrdw(alloc, &tx, blockchainCommunityIdIndex);

						uint8_t staticResultBuffer[1024];
						auto encodeResult = grdw_confirmed_transaction_encode(&tx, staticResultBuffer, 1024);
						if (GRDW_ENCODING_ERROR_OUT_OF_MEMORY == encodeResult.state) {
							Block resultBuffer(2048);
							encodeResult = grdw_confirmed_transaction_encode(&tx, resultBuffer, 2048);
							if (GRDW_ENCODING_ERROR_SUCCESS == encodeResult.state) {
								LOG_F(WARNING, "static output buffer was to small, used 2048 Bytes buffer, acutally used: %d Bytes", encodeResult.written);
								return make_shared<const Block>(encodeResult.written, resultBuffer);
							}
						}
						if (GRDW_ENCODING_ERROR_SUCCESS != encodeResult.state) {
							LOG_F(ERROR, "encode error: %s", enum_name(encodeResult.state).data());
							throw GradidoNodeInvalidDataException("error serialize confirmed transaction");
						}
						return make_shared<const Block>(encodeResult.written, staticResultBuffer);
				});
			*/

  ConstBlockPtr TransactionBodyZigRole::run() const 
  { 
		GrduStaticBuffer<1024> staticInputBuffer;
		return staticInputBuffer.use([&](grdu_memory* alloc) -> ConstBlockPtr
			{
				grdw_transaction_body body;
				mBody.toGrdw(alloc, &body);

				uint8_t staticResultBuffer[1024];
				auto encodeResult = grdw_transaction_body_encode(&body, staticResultBuffer, 1024);
				if (GRDW_ENCODING_ERROR_OUT_OF_MEMORY == encodeResult.state) {
					Block resultBuffer(2048);
					encodeResult = grdw_transaction_body_encode(&body, resultBuffer, 2048);
					if (GRDW_ENCODING_ERROR_SUCCESS == encodeResult.state) {
						LOG_F(WARNING, "static output buffer was to small, used 2048 Bytes buffer, acutally used: %d Bytes", encodeResult.written);
						return make_shared<const Block>(encodeResult.written, resultBuffer);
					}
				}
				if (GRDW_ENCODING_ERROR_SUCCESS != encodeResult.state) {
					LOG_F(ERROR, "encode error: %s", enum_name(encodeResult.state).data());
					throw GradidoNodeInvalidDataException("error serialize confirmed transaction");
				}
				return make_shared<const Block>(encodeResult.written, staticResultBuffer);
			}
		);
  }
    
  size_t TransactionBodyZigRole::calculateSerializedSize() const 
  {
    return 0;
  }
}
