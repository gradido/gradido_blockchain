#ifndef __GRADIDO_BLOCKCHAIN_HTTP_SERVER_EXCEPTIONS_H
#define __GRADIDO_BLOCKCHAIN_HTTP_SERVER_EXCEPTIONS_H

#include "gradido_blockchain/GradidoBlockchainException.h"

class ServerAlreadyRunningException : public GradidoBlockchainException
{
public: 
	explicit ServerAlreadyRunningException(const char* what) noexcept : GradidoBlockchainException(what) {}
	std::string getFullString() const { return what(); }
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_SERVER_EXCEPTIONS_H