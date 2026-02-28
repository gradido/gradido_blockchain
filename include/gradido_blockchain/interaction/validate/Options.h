#ifndef GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_OPTIONS_H
#define GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_OPTIONS_H

#include "Type.h"

namespace gradido::interaction::validate {
  struct Options {
      Type type = Type::SINGLE;
      bool enableVerify = true; 
      bool enableRunningHashTest = true;
      bool errorOnMissingColdData = false;
  };
}

#endif //GRADIDO_BLOCKCHAIN_INTERACTION_VALIDATE_OPTIONS_H