#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_TYPE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_TYPE_H

#include "gradido_blockchain/types.h"

namespace gradido::interaction::validate {
  enum class ErrorType: uint8_t {
    NONE,
    SUCCESS,    
  };
}

#endif // GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_TYPE_H