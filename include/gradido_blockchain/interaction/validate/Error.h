#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_H 

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/interaction/validate/ErrorType.h"

namespace gradido::interaction::validate {
  struct GRADIDOBLOCKCHAIN_EXPORT Error
  {
    ErrorType type = ErrorType::None;
    std::string message;
    std::string actual;
    std::string expected;
  };
}

#endif // GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_H