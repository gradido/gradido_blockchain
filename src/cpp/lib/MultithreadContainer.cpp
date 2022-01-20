#include "MultithreadContainer.h"
#include "NotificationList.h"

#include <thread>
#include <chrono>

namespace UniLib {
	namespace lib {

		void MultithreadContainer::lock(const char* stackDetails/* = nullptr*/)
		{
			using namespace std::chrono_literals;
			const static char* functionName = "MultithreadContainer::lock";
			
			uint8_t exitCounter = 5;
			while (!mWorkMutex.try_lock() && exitCounter > 0) {
				std::this_thread::sleep_for(100ms);
				exitCounter--;
			}
			if (exitCounter > 0) {
				if (stackDetails) {
					mLastSucceededLock = stackDetails;
				}
			}
			else {
				NotificationList errors;
				errors.addError(new Error(functionName, "lock timeout"));
				if (mLastSucceededLock != "") {
					errors.addError(new ParamError(functionName, "last succeed lock by ", mLastSucceededLock.data()));
				}
				if (stackDetails) {
					errors.addError(new Error(functionName, stackDetails));
				}
			}			
		}

		bool MultithreadContainer::tryLock()
		{
			return mWorkMutex.try_lock();
		}
	}
}