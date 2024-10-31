#include "gradido_blockchain/interaction/calculateCreationSum/AbstractAlgoRole.h"
#include "gradido_blockchain/interaction/calculateCreationSum/Context.h"
#include "gradido_blockchain/interaction/calculateCreationSum/V01_ThreeMonthsAlgoRole.h"
#include "gradido_blockchain/interaction/calculateCreationSum/V02_TargetDateAlgoRole.h"

#include <chrono>

using namespace std::chrono;

namespace gradido {
	namespace interaction {
		namespace calculateCreationSum {
			Context::Context(Timepoint date, Timepoint targetDate, memory::ConstBlockPtr publicKey, uint64_t transactionNrMax/* = 0 */)
			{
				auto secondsSinceEpoch = time_point_cast<seconds>(date).time_since_epoch().count();
				if (secondsSinceEpoch < 1588503608) {
					mRole = std::make_unique<V01_ThreeMonthsAlgoRole>(date, targetDate, publicKey, transactionNrMax);
				}
				else {
					mRole = std::make_unique<V02_TargetDateAlgoRole>(date, targetDate, publicKey, transactionNrMax);
				}
			}
		}
	}
}

