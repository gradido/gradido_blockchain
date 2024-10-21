#ifndef __GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H
#define __GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H

#include "gradido_blockchain/export.h"

#include <atomic>

class GRADIDOBLOCKCHAIN_EXPORT ServerApplication
{
public:
	ServerApplication();
	//! \return true on success, false on failure, if return false, exit will be called and program will end
	virtual bool init() = 0;
	void run();
	virtual void exit() = 0;
	static void terminate() { gRunning = false; }
protected:
	static std::atomic<bool> gRunning;
};

#endif //__GRADIDO_BLOCKCHAIN_SERVER_APPLICATION_H