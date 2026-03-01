#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_TYPE_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_TYPE_H

#include "gradido_blockchain/types.h"

namespace gradido::interaction::validate {
  enum class ErrorType: uint8_t {
    None,
    Success,
    Invalid_Field,
    Invalid_Dictionary_Index,
    Invalid_Transaction_Type,
    Field_Value_Conflict,
    Missing_Cold_Data,
    Crypto_Sign_Invalid
  };
}

#endif // GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_TYPE_H