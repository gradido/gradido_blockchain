#ifndef __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H

#include "Poco/Net/Context.h"

namespace ServerConfig
{
	extern Poco::Net::Context::Ptr g_SSL_Client_Context;

	bool initSSLClientContext(const char* cacertPath);
}

#endif //__GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H