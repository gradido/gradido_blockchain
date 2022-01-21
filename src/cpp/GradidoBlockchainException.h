#ifndef __GRADIDO_BLOCKCHAIN_EXCEPTION_H
#define __GRADIDO_BLOCKCHAIN_EXCEPTION_H

#include <stdexcept>

class GradidoBlockchainException : public std::runtime_error
{
public:
	explicit GradidoBlockchainException(const char* what) : std::runtime_error(what) {}
	virtual ~GradidoBlockchainException() {};
	virtual std::string getFullString() const = 0;
};

#endif __GRADIDO_BLOCKCHAIN_EXCEPTION_H