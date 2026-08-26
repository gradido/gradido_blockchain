#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TIMESTAMP_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TIMESTAMP_H

#include "gradido_blockchain_core/data/timestamp.h"
#include "gradido_blockchain_core/data/wire/basic_types.h"
#include "gradido_blockchain/data/Timestamp.h"
#include "gradido_blockchain/data/TimestampSeconds.h"
#include "gradido_blockchain/data/DurationSeconds.h"

namespace gradido::data::adapter {
  inline Timestamp fromGrdw(grdd_timestamp timestamp) {
    return {timestamp.seconds, timestamp.nanos};
  }
  inline grdd_timestamp toGrdw(Timestamp timestamp) {
    return {.seconds = timestamp.getSeconds(), .nanos = timestamp.getNanos()};
  }
  inline TimestampSeconds fromGrdw(grdw_timestamp_seconds timestamp) {
    return {timestamp.seconds};
  }
  inline grdw_timestamp_seconds toGrdw(TimestampSeconds timestamp) {
    return {.seconds = timestamp.getSeconds()};
  }
}

#endif //  __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_TIMESTAMP_H