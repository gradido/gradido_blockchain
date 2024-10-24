#include "gradido_blockchain/http/HttpRequest.h"
#include "gradido_blockchain/http/RequestExceptions.h"
#include "gradido_blockchain/http/ServerConfig.h"


#include "furi/furi.hpp"
#include "magic_enum/magic_enum.hpp"
#include "loguru/loguru.hpp"

#ifdef USE_HTTPS
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include "httplib.h"

using namespace rapidjson;

static httplib::Headers constructHeaders(std::multimap<std::string, std::string> headers, std::multimap<std::string, std::string> cookies) 
{
	httplib::Headers httplib_headers;
	for (auto& headerPair : headers) {
		httplib_headers.insert(headerPair);
	}
	if (cookies.size()) {
		std::ostringstream cookie_stream;
		for (const auto& [key, value] : cookies) {
			cookie_stream << key << "=" << value << ";";
		}
		std::string cookie_header = cookie_stream.str();
		cookie_header.pop_back(); // remove last semikolon

		// add cookie header 
		httplib_headers.emplace("Cookie", cookie_header);
	}
	return httplib_headers;
}

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

std::string HttpRequest::POST(const std::string& body, const char* contentType/* = "application/json"*/, const char* path/* = nullptr*/)
{
	httplib::Client cli(constructHostString());
	auto uri = furi::uri_split::from_uri(mUrl);	

	std::string finalPath;
	if (!path) {
		finalPath = uri.path;
	}
	else {
		finalPath = path;
	}

	auto res = cli.Post(finalPath.data(), constructHeaders(mHeaders, mCookies), body, contentType);
	if (res->status != 200) {
		throw HttplibRequestException("status isn't 200 for POST", mUrl, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

std::string HttpRequest::GET(const std::map<std::string, std::string> query, const char* path/* = nullptr*/)
{
	httplib::Client cli(constructHostString());
	auto uri = furi::uri_split::from_uri(mUrl);
	std::string pathAndQuery("/");
	
	if (!path) {
		if (uri.path.size()) {
			pathAndQuery += std::string(uri.path.data());
		}
	}
	else {
		pathAndQuery = path;
	}
	if (query.empty()) {
		if (uri.query.size()) {
			pathAndQuery += "?" + std::string(uri.query.data());
		}
	}
	else {
		pathAndQuery += "?";
		for (auto& pair : query) {
			pathAndQuery += pair.first + "=" + pair.second;
			pathAndQuery += "&";
		}
		// remove last &
		pathAndQuery.pop_back();
	}
	auto res = cli.Get(pathAndQuery, constructHeaders(mHeaders, mCookies));
	if (res->status != 200) {
		throw HttplibRequestException("status isn't 200 for GET", mUrl, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

std::string HttpRequest::GET(const char* path)
{
	httplib::Client cli(constructHostString());
	
	auto res = cli.Get(path, constructHeaders(mHeaders, mCookies));
	if (res->status != 200) {
		throw HttplibRequestException("status isn't 200 for GET", mUrl, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

std::string HttpRequest::constructHostString()
{
	auto uri = furi::uri_split::from_uri(mUrl);
	// http | https
	std::string host = uri.scheme.data();
	if (host.empty()) {
		host += "http";
	}
#ifndef USE_HTTPS
	if ("https" == host) {
		host = "http";
		LOG_F(WARNING, "try to make Https Request but cpp-httplib was included without OpenSSL-Support, changed to http");
	}
#endif
	// host:port
	host += "://" + std::string(uri.authority.data());
	return host;
}
