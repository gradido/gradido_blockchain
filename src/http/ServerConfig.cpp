
#include "gradido_blockchain/http/ServerConfig.h"

namespace ServerConfig {
	AllowUnsecure g_AllowUnsecureFlags = NOT_UNSECURE;
	IotaRequest* g_IotaRequestHandler = nullptr;
	bool		 g_IotaLocalPow = true;
	/*
	bool initSSLClientContext(const char* cacertPath)
	{
		SharedPtr<InvalidCertificateHandler> pCert = new RejectCertificateHandler(false); // reject invalid certificates
		try {
#ifdef POCO_NETSSL_WIN
			g_SSL_Client_Context = new Context(Context::CLIENT_USE, cacertPath, Context::VERIFY_RELAXED, Context::OPT_DEFAULTS);
#else

			g_SSL_Client_Context = new Context(Context::CLIENT_USE, "", "", cacertPath, Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
#endif
		}
		catch (Poco::Exception& ex) {
			printf("[ServerConfig::initSSLClientContext] error init ssl context, maybe no cacert.pem found?\nPlease make sure you have cacert.pem (CA/root certificates) next to binary from https://curl.haxx.se/docs/caextract.html\n");
			return false;
		}
		SSLManager::instance().initializeClient(0, pCert, g_SSL_Client_Context);


		return true;
	}
	*/
	bool readUnsecureFlags(const MapEnvironmentToConfig& cfg)
	{
		// unsecure flags
		//g_AllowUnsecureFlags
		if (cfg.getBool("unsecure.allow_passwort_via_json_request", false)) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_PASSWORD_REQUESTS);
		}
		if (cfg.getBool("unsecure.allow_auto_sign_transactions", false)) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_AUTO_SIGN_TRANSACTIONS);
		}
		if (cfg.getBool("unsecure.allow_cors_all", false)) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_CORS_ALL);
		}
		if (cfg.getBool("unsecure.allow_all_passwords", false)) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_ALLOW_ALL_PASSWORDS);
		}
		return true;
	}

	bool initIota(const MapEnvironmentToConfig& cfg)
	{
		std::string iota_host = cfg.getString("iota.host", "api.lb-0.h.chrysalis-devnet.iota.cafe");
		int iota_port = cfg.getInt("iota.port", 443);
		g_IotaLocalPow = cfg.getBool("iota.local_pow", g_IotaLocalPow);
		g_IotaRequestHandler = new IotaRequest(iota_host, iota_port, "/api/v1/");

		return true;
	}

	
}