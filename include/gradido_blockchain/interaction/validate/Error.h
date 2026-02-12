#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_H

#include "ResultType.h"
#include "gradido_blockchain/export.h"

#include <string>

namespace gradido::interaction::validate {
  class GRADIDOBLOCKCHAIN_EXPORT Error 
  { 
  public:
    Error();
    virtual ~Error() = default;
    
    Error(
      const char* what,
      const char* fieldName,
      const char* fieldType = nullptr,
      const char* expected = nullptr,
      const char* actual = nullptr
    );
    
  protected:
    std::string mErrorMessage;
    ResultType mResultType;
  };
}
#endif // GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_ERROR_H