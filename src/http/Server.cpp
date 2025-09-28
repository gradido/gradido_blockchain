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
) :
	mHost(host),
	mPort(port),
	mName(name),
	mServer(new httplib::SSLServer(sslCertificate.data(), sslKey.data())),
	mServerThread(nullptr)
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
		//	LOG_F(INFO, "file requested: %s", req.path.data());
		res.status = httplib::StatusCode::BadRequest_400;
		res.set_content("File not Found", "text/plain");
		});
	mServer->set_logger([](const httplib::Request& req, const httplib::Response& res) {
		//LOG_F(INFO, "path requested: %s, method: %s", req.path.data(), req.method.data());
		//printf("request: %s, response: %s\n", req.body.data(), res.body.data());
		});
	mServer->set_error_handler([this](const auto& req, auto& res) {
		// check path
		auto it = mRegisteredResponseHandlers.find(req.path);
		if (it != mRegisteredResponseHandlers.end()) {
			res.status = httplib::StatusCode::NotFound_404;
			std::string response = "Path: " + req.path + " Not Found";
			res.set_content(response, "text/plain");
			return;
		}
		std::string data = req.path;
		if (!req.body.empty()) {
			data = req.body;
		}
		LOG_F(ERROR, "error response for request: %s", data.data());
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

	auto registerMethod = [](
		const httplib::Request& req,
		httplib::Response& res,
		MethodType methodType,
		std::multimap<std::string, AbstractResponseHandlerFactory*> registeredResponseHandlers
		) {
			auto range = registeredResponseHandlers.equal_range(req.path);
			for (auto i = range.first; i != range.second; ++i) {
				if (i->second->has(methodType)) {
					auto handler = i->second->getResponseHandler(methodType);
					handler->handleRequest(req, res, methodType);
					break;
				}
			}
		};
	mServer->Get(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		std::lock_guard _lock(mMapMutex);
		registerMethod(req, res, MethodType::GET, mRegisteredResponseHandlers);
	});
	mServer->Post(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		std::lock_guard _lock(mMapMutex);
		registerMethod(req, res, MethodType::POST, mRegisteredResponseHandlers);
	});
	mServer->Put(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		std::lock_guard _lock(mMapMutex);
		registerMethod(req, res, MethodType::PUT, mRegisteredResponseHandlers);
	});
	mServer->Options(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		std::lock_guard _lock(mMapMutex);
		registerMethod(req, res, MethodType::OPTIONS, mRegisteredResponseHandlers);
	});
	mServer->Delete(pathName, [&](const httplib::Request& req, httplib::Response& res) {
		std::lock_guard _lock(mMapMutex);
		registerMethod(req, res, MethodType::DEL, mRegisteredResponseHandlers);
	});
}

