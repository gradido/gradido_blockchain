#include "gradido_blockchain_core/data/wire/transaction_body.h"
#include "gradido_blockchain_core/memory.h"
#include "gradido_blockchain/data/TransactionBody.h"
#include "gradido_blockchain/interaction/serialize/TransactionBodyRole.h"
#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/memory/grdu_StaticBuffer.h"

#include "loguru/loguru.hpp"
#include "magic_enum/magic_enum.hpp"

#include <memory>

using namespace magic_enum;
using memory::Block, memory::BlockPtr, memory::ConstBlockPtr, memory::GrduStaticBuffer;
using std::make_shared;

namespace gradido::interaction::serialize {

	ConstBlockPtr TransactionBodyRole::run() const
  {
		GrduStaticBuffer<1024> staticInputBuffer;
		BlockPtr resultPtr = nullptr;
		staticInputBuffer.use([&](grd_memory* alloc) -> grd_result
			{
				grdw_transaction_body body;

				mBody.toGrdw(alloc, &body);

				uint8_t staticResultBuffer[1024];
				grd_memory_block resultBuffer = { .data = staticResultBuffer, .size = 1024 };
				size_t finalSize = 0;
				auto encodeResult = grdw_transaction_body_encode(&resultBuffer, &finalSize, &body, alloc);
				if (GRD_ERROR_DESTINATION_BUFFER_TO_SMALL == encodeResult) {
					Block dynamicResultBuffer(4096);
					resultBuffer.data = dynamicResultBuffer.data();
					resultBuffer.size = dynamicResultBuffer.size();
					encodeResult = grdw_transaction_body_encode(&resultBuffer, &finalSize, &body, alloc);
					if (GRD_SUCCESS == encodeResult) {
						LOG_F(WARNING, "static output buffer was to small, used %lu Bytes buffer, actually used: %lu Bytes", resultBuffer.size, finalSize);
						resultPtr = make_shared<Block>(finalSize, dynamicResultBuffer);
						return GRD_SUCCESS;
					}
				}
				if (GRD_ERROR_OUT_OF_MEMORY == encodeResult) {
					return encodeResult;
				}
				if (GRD_SUCCESS != encodeResult) {
					LOG_F(ERROR, "encode error: %s", enum_name(encodeResult).data());
					throw GradidoNodeInvalidDataException("error serialize confirmed transaction");
				}
				resultPtr = make_shared<Block>(finalSize, staticResultBuffer);
				return encodeResult;
			}
		);
		return resultPtr;
  }

  size_t TransactionBodyRole::calculateSerializedSize() const 
  {
    return 0;
  }
}
