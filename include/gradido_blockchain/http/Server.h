#ifndef __GRADIDO_BLOCKCHAIN_HTTP_SERVER_H
#define __GRADIDO_BLOCKCHAIN_HTTP_SERVER_H

#include "gradido_blockchain/export.h"
#include "AbstractResponseHandlerFactory.h"

#include <string_view>
#include <map>
#include <mutex>
#include <thread>
#include <functional>

namespace httplib {
	class Server;
}


class GRADIDOBLOCKCHAIN_EXPORT Server
{
public:
	Server(std::string_view host, int port, std::string_view name);
#ifdef USE_HTTPS
	Server(
		std::string_view host,
		int port,
		std::string_view name,
		std::string_view sslCertificate,
		std::string_view sslKey
	);
#endif
	~Server();

	void registerResponseHandler(const std::string& pathName, AbstractResponseHandlerFactory* factory);

	//! prepare everything for server start
	bool init();

	//! start http or https server
	void run();
	
	//! stop server and clean up
	void exit();
protected:
	void registerPath(const std::string& pathName);
	
	//! path for example "/" and ResponseHandlerFactory
	std::multimap<std::string, AbstractResponseHandlerFactory*> mRegisteredResponseHandlers;
	std::mutex mMapMutex;

	std::string mHost;
	int mPort;	
	std::string mName;
	httplib::Server* mServer;
	std::thread* mServerThread;
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_SERVER_H