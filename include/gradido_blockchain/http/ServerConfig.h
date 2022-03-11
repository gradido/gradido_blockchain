#ifndef __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H

#include "Poco/Net/Context.h"
#include "gradido_blockchain/export.h"

namespace ServerConfig
{
	GRADIDOBLOCKCHAIN_EXPORT extern Poco::Net::Context::Ptr g_SSL_Client_Context;
	GRADIDOBLOCKCHAIN_EXPORT bool initSSLClientContext(const char* cacertPath);
}

#endif //__GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H