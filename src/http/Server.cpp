#include "gradido_blockchain/http/Server.h"
#include "gradido_blockchain/http/ServerExceptions.h"

#ifdef USE_HTTPS
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include "cpp-httplib/httplib.h"
#include "loguru/loguru.hpp"


Server::Server(std::string_view host, int port, std::string_view name)
	: mHost(host), mPort(port), mName(name), mServer(new httplib::Server), mServerThread(nullptr)
{
}

#ifdef USE_HTTPS
Server::Server(
	std::string_view host,
	int port,
	std::string_view name,
	std::string_view sslCertificate,
	std::string_view sslKey
) : mHost(host), mPort(port), mName(name), mServer(new httplib::SSLServer(sslCertificate, sslKey)), mServerThread(nullptr)
{

}
#endif

Server::~Server()
{
	exit();		
}

void Server::registerResponseHandler(const std::string& pathName, AbstractResponseHandlerFactory* factory)
{
	std::lock_guard _lock(mMapMutex);
	if (mServerThread) {
		throw ServerAlreadyRunningException("cannot add response handler, after server was started!");
	}
	auto it = mRegisteredResponseHandlers.find(pathName);
	if (it == mRegisteredResponseHandlers.end()) {
		registerPath(pathName);
	}
	mRegisteredResponseHandlers.insert({ pathName, factory });
}

bool Server::init()
{
	mServer->set_file_request_handler([](const httplib::Request& req, httplib::Response& res) {
		res.status = httplib::StatusCode::BadRequest_400;
		res.set_content("File not Found", "text/plain");
	});
	mServer->set_logger([](const httplib::Request& req, const httplib::Response& res) {
		printf("request: %s, response: %s\n", req.body.data(), res.body.data());
	});
	mServer->set_error_handler([](const auto& req, auto& res) {
		LOG_F(ERROR, "error response for request: %s", req.body.data());
	});	
	return true;
}

void Server::run()
{
	mServerThread = new std::thread([&]() {
		loguru::set_thread_name(mName.data());
		mServer->listen(mHost, mPort); 
	});
}


void Server::exit()
{	
	if (mServer) {
		mServer->stop();
		if (mServerThread) {
			mServerThread->join();
			delete mServerThread;
			mServerThread = nullptr;
		}
		delete mServer;
		mServer = nullptr;
	}

	std::lock_guard _lock(mMapMutex);
	for (auto& pair : mRegisteredResponseHandlers) {
		delete pair.second;
	}
	mRegisteredResponseHandlers.clear();
}



void Server::registerPath(const std::string& pathName)
{
	auto registerMethod = [&](const httplib::Request& req, httplib::Response& res, MethodType methodType) {
		std::lock_guard _lock(mMapMutex);
		auto range = mRegisteredResponseHandlers.equal_range(pathName);
		for (auto i = range.first; i != range.second; ++i) {
			if (i->second->has(methodType)) {
				auto handler = i->second->getResponseHandler(methodType);
				handler->handleRequest(req, res, methodType);
			}
		}
	};
	mServer->Get(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		registerMethod(req, res, MethodType::GET);
	});
	mServer->Post(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		registerMethod(req, res, MethodType::POST);
	});
	mServer->Put(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		registerMethod(req, res, MethodType::PUT);
	});
	mServer->Options(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		registerMethod(req, res, MethodType::OPTIONS);
	});
	mServer->Delete(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		registerMethod(req, res, MethodType::DEL);
	});
	
}

