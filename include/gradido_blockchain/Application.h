#ifndef __GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H
#define __GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H

#include "gradido_blockchain/export.h"

#include <atomic>
#include <condition_variable>
#include <stop_token>

/*
* Class with loop for longer running Apps
* Only one instance should be used!
*/
class GRADIDOBLOCKCHAIN_EXPORT Application
{
public:
	Application();
	virtual ~Application();
	//! call init and start main loop if init return true
	virtual void run();

	static void terminate() { mMasterStopSource.request_stop(); gRunning = false; mExitCondition.notify_one(); }
	static std::atomic<bool> gRunning;
	std::stop_token getStopToken() const { return mMasterStopSource.get_token(); }
protected:
	//! \return true on success, false on failure, if return false, exit will be called and program will end
	virtual bool init() = 0;
	virtual void exit() = 0;
		
	static std::mutex mConditionMutex;
	static std::condition_variable mExitCondition;
	static std::stop_source mMasterStopSource;	
};

#endif //__GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H