
#include "gradido_blockchain/http/ServerConfig.h"

#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/RejectCertificateHandler.h"
#include "Poco/Net/DNS.h"
#include "Poco/SharedPtr.h"

using Poco::Net::SSLManager;
using Poco::Net::Context;
using Poco::Net::KeyConsoleHandler;
using Poco::Net::PrivateKeyPassphraseHandler;
using Poco::Net::InvalidCertificateHandler;
using Poco::Net::RejectCertificateHandler;
using Poco::SharedPtr;

namespace ServerConfig {
	Context::Ptr g_SSL_Client_Context = nullptr;
	AllowUnsecure g_AllowUnsecureFlags = NOT_UNSECURE;
	IotaRequest* g_IotaRequestHandler = nullptr;
	bool		 g_IotaLocalPow = true;

	bool initSSLClientContext(const char* cacertPath)
	{
		SharedPtr<InvalidCertificateHandler> pCert = new RejectCertificateHandler(false); // reject invalid certificates
		/*
		Context(Usage usage,
		const std::string& certificateNameOrPath,
		VerificationMode verMode = VERIFY_RELAXED,
		int options = OPT_DEFAULTS,
		const std::string& certificateStoreName = CERT_STORE_MY);
		*/
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

	bool readUnsecureFlags(const Poco::Util::LayeredConfiguration& cfg)
	{
		// unsecure flags
		//g_AllowUnsecureFlags
		if (cfg.getInt("unsecure.allow_passwort_via_json_request", 0) == 1) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_PASSWORD_REQUESTS);
		}
		if (cfg.getInt("unsecure.allow_auto_sign_transactions", 0) == 1) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_AUTO_SIGN_TRANSACTIONS);
		}
		if (cfg.getInt("unsecure.allow_cors_all", 0) == 1) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_CORS_ALL);
		}
		if (cfg.getInt("unsecure.allow_all_passwords", 0) == 1) {
			g_AllowUnsecureFlags = (AllowUnsecure)(g_AllowUnsecureFlags | UNSECURE_ALLOW_ALL_PASSWORDS);
		}
		return true;
	}

	bool initIota(const Poco::Util::LayeredConfiguration& cfg)
	{
		std::string iota_host = cfg.getString("iota.host", "api.lb-0.h.chrysalis-devnet.iota.cafe");
		int iota_port = cfg.getInt("iota.port", 443);
		g_IotaLocalPow = cfg.getBool("iota.local_pow", g_IotaLocalPow);
		g_IotaRequestHandler = new IotaRequest(iota_host, iota_port, "/api/v1/");

		return true;
	}

	
}