#include "gradido_blockchain/http/HttpRequest.h"
#include "gradido_blockchain/http/RequestExceptions.h"

#include "gradido_blockchain/http/ServerConfig.h"
#include "magic_enum/magic_enum.hpp"

#include "furi/furi.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

using namespace rapidjson;

HttpRequest::HttpRequest(const std::string& url)
	: mUrl(url)
{
}

HttpRequest::HttpRequest(const std::string& host, int port, const char* path/* = nullptr*/, const char* query/* = nullptr*/)
{	
	mUrl = host + ":" + std::to_string(port);

	if (path) {
		mUrl += "/" + std::string(path);
	}
	if (query) {
		mUrl += "?" + std::string(query);
	}
}

std::string HttpRequest::GET(const std::string& _pathAndQuery)
{
	auto uri = furi::uri_split::from_uri(mUrl);
						// http | https
	std::string host = uri.scheme.data();
								// host:port
	host += "://" + std::string(uri.authority.data());

	httplib::Client cli(host);
	std::string pathAndQuery;
	
	if (_pathAndQuery.empty()) {
		
		pathAndQuery = "/";
		pathAndQuery += std::string(uri.path.data());
		if (uri.query.size()) {
			pathAndQuery += "?" + std::string(uri.query.data());
		}
	}
	auto res = cli.Get(pathAndQuery);
	if (res->status != 200) {
		throw HttplibRequestException("status isn't 200 for GET", mUrl, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

