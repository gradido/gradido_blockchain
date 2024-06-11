#ifndef __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_ABSTRACT_ROLE_H
#define __GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_ABSTRACT_ROLE_H

namespace gradido {
	namespace v3_3 {
		namespace interaction {
			namespace validate {
        class AbstractRole 
        {
        public:
          virtual ~AbstractRole() {}
          virtual void prepare() {};
          // test if transaction is valid, throw an exception on error
          virtual void run() = 0;

        };
      }
    }
  }
}



#endif //__GRADIDO_BLOCKCHAIN_V3_3_INTERACTION_VALIDATE_ABSTRACT_ROLE_H