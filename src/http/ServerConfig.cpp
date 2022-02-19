
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

	
}