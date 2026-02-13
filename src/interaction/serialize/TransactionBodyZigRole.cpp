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

	ConstBlockPtr TransactionBodyZigRole::run() const
  {
		GrduStaticBuffer<1024> staticInputBuffer;
		return staticInputBuffer.use([&](grdu_memory* alloc) -> ConstBlockPtr
			{
				grdw_transaction_body body;

				mBody.toGrdw(alloc, &body);

				uint8_t staticResultBuffer[2048];
				auto encodeResult = grdw_transaction_body_encode(&body, staticResultBuffer, 2048);
				if (GRDW_ENCODING_ERROR_OUT_OF_MEMORY == encodeResult.state || GRDW_ENCODING_ERROR_WRITE_FAILED == encodeResult.state) {
					Block resultBuffer(4096);
					encodeResult = grdw_transaction_body_encode(&body, resultBuffer, resultBuffer.size());
					if (GRDW_ENCODING_ERROR_SUCCESS == encodeResult.state) {
						LOG_F(WARNING, "static output buffer was to small, used %lu Bytes buffer, actually used: %d Bytes", resultBuffer.size(), encodeResult.written);
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
