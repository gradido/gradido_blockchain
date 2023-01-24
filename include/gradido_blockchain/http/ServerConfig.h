#ifndef __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H

#include "Poco/Net/Context.h"
#include "gradido_blockchain/lib/MapEnvironmentToConfig.h"
#include "gradido_blockchain/export.h"
#include "gradido_blockchain/http/IotaRequest.h"


namespace ServerConfig
{
	// used with bit-operators, so only use numbers with control exactly one bit (1,2,4,8,16...)
	enum GRADIDOBLOCKCHAIN_EXPORT AllowUnsecure {
		NOT_UNSECURE = 0,
		UNSECURE_PASSWORD_REQUESTS = 1,
		UNSECURE_AUTO_SIGN_TRANSACTIONS = 2,
		UNSECURE_CORS_ALL = 4,
		UNSECURE_ALLOW_ALL_PASSWORDS = 8
	};

	GRADIDOBLOCKCHAIN_EXPORT extern AllowUnsecure g_AllowUnsecureFlags;
	GRADIDOBLOCKCHAIN_EXPORT extern Poco::Net::Context::Ptr g_SSL_Client_Context;
	GRADIDOBLOCKCHAIN_EXPORT extern IotaRequest* g_IotaRequestHandler;
	GRADIDOBLOCKCHAIN_EXPORT extern bool		 g_IotaLocalPow;

	GRADIDOBLOCKCHAIN_EXPORT bool initSSLClientContext(const char* cacertPath);
	GRADIDOBLOCKCHAIN_EXPORT bool readUnsecureFlags(const MapEnvironmentToConfig& config);
	GRADIDOBLOCKCHAIN_EXPORT bool initIota(const MapEnvironmentToConfig& cfg);
}

#endif //__GRADIDO_BLOCKCHAIN_HTTP_SERVER_CONFIG_H