#include "gradido_blockchain/interaction/validate/Error.h"

namespace gradido::interaction::validate {
  Error::Error()
  : mResultType(ResultType::SUCCESS) 
  {
  }
  Error::Error(
    const char* what,
    const char* fieldName,
    const char* fieldType /*= nullptr*/,
    const char* expected /*= nullptr*/,
    const char* actual /*= nullptr*/
  ) {
    
  }
}
