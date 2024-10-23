#ifndef __GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H
#define __GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H

#include "gradido_blockchain/export.h"

#include <atomic>

/*
* Class with loop for longer running Apps
*/
class GRADIDOBLOCKCHAIN_EXPORT Application
{
public:
	Application();
	virtual ~Application();
	//! call init and start main loop if init return true
	virtual void run();
	
	static void terminate() { gRunning = false; }
protected:
	//! \return true on success, false on failure, if return false, exit will be called and program will end
	virtual bool init() = 0;
	virtual void exit() = 0;

	static std::atomic<bool> gRunning;
};

#endif //__GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H