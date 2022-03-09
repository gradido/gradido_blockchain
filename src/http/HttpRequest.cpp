#include "gradido_blockchain/http/HttpRequest.h"
#include "gradido_blockchain/http/RequestExceptions.h"

#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Exception.h"

#include "gradido_blockchain/http/ServerConfig.h"


using namespace rapidjson;

HttpRequest::HttpRequest(const std::string& url)
	: mRequestUri(url)
{
}
HttpRequest::HttpRequest(const Poco::URI& requestUri)
	: mRequestUri(requestUri)
{

}
HttpRequest::HttpRequest(const std::string& host, int port, const char* path/* = nullptr*/, const char* query/* = nullptr*/)
{
	mRequestUri.setHost(host);
	mRequestUri.setPort(port);
	if (path) {
		mRequestUri.setPath(path);
	}
	if (query) {
		mRequestUri.setQuery(query);
	}
}

Poco::SharedPtr<Poco::Net::HTTPClientSession> HttpRequest::createClientSession()
{
	assert(!mRequestUri.getHost().empty() && mRequestUri.getPort());
	
	Poco::SharedPtr<Poco::Net::HTTPClientSession> clientSession;
	if (mRequestUri.getPort() == 443) {
		clientSession = new Poco::Net::HTTPSClientSession(mRequestUri.getHost(), mRequestUri.getPort(), ServerConfig::g_SSL_Client_Context);
	}
	else {
		clientSession = new Poco::Net::HTTPClientSession(mRequestUri.getHost(), mRequestUri.getPort());
	}
	return clientSession;	
}

std::string HttpRequest::GET(const char* pathAndQuery/* = nullptr*/, const char* version/* = nullptr*/)
{
	auto clientSession = createClientSession();
	
	if (!pathAndQuery) {
		pathAndQuery = mRequestUri.getPathAndQuery().data();
	}
	std::string _version = "HTTP/1.0";
	if (version) _version = version;
	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, pathAndQuery, _version);

	std::string responseString;
	request.setChunkedTransferEncoding(true);
	try {
		std::ostream& request_stream = clientSession->sendRequest(request);

		Poco::Net::HTTPResponse response;
		std::istream& response_stream = clientSession->receiveResponse(response);

		for (std::string line; std::getline(response_stream, line); ) {
			responseString += line + "\n";
		}
	}
	catch (Poco::Exception& ex) {
		throw PocoNetException(ex, mRequestUri, pathAndQuery);
	}

	return responseString;
}

