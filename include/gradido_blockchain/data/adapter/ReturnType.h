#ifndef __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_RETURN_TYPE_H
#define __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_RETURN_TYPE_H

namespace gradido::data::adapter {
  enum class ReturnType {
    SUCCESS = 0,
    ERROR = -1,
    INVALID_VERSION = -2,
  };
}

#endif // __GRADIDO_BLOCKCHAIN_DATA_ADAPTER_RETURN_TYPE_H