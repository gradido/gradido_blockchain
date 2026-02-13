#include "gradido_blockchain/blockchain/batch/ThreadingPolicy.h"

#include <thread>
#include <algorithm>

using std::max;

namespace gradido::blockchain::batch {
  size_t resolveThreadCount(ThreadingPolicy policy) 
  {
    const auto hw = std::thread::hardware_concurrency();
    if (hw == 0) return 1;

    switch (policy) {
      case ThreadingPolicy::SingleThread:     return 1;
      case ThreadingPolicy::Quarter:          return max<size_t>(1, hw / 4);
      case ThreadingPolicy::Half:             return max<size_t>(1, hw / 2);
      case ThreadingPolicy::ThreeQuarter:     return max<size_t>(1, (hw * 3) / 4);
      case ThreadingPolicy::All:              return hw;
      case ThreadingPolicy::AllExceptOne:     return hw > 1 ? hw - 1 : 1;
    }
    return 1;
  }    
}
