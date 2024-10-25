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
	auto uri = furi::uri_split::from_uri(mUrl);
	if (uri.scheme != "http" && uri.scheme != "https") {
		throw RequestException("cannot find scheme (http|https) in url", url);
	}
	if (uri.authority.empty()) {
		throw RequestException("cannot find host in url, please use something like: http://server.com:80", url);
	}
}

HttpRequest::HttpRequest(const std::string& host, int port, const char* path/* = nullptr*/, const char* query/* = nullptr*/)
{
	if (host.find("http") == std::string::npos) {
		if (port == 443) {
			mUrl = "https://";
		}
		else {
			mUrl = "http://";
		}
	}
	mUrl += host + ":" + std::to_string(port);

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
	if (!res) {
		throw HttplibRequestException("no response", mUrl, 0, magic_enum::enum_name(res.error()).data());
	}
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
	if (!res) {
		throw HttplibRequestException("no response", mUrl, 0, magic_enum::enum_name(res.error()).data());
	}
	if (res->status != 200) {
		throw HttplibRequestException("status isn't 200 for GET", mUrl, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

std::string HttpRequest::GET(const char* path)
{
	httplib::Client cli(constructHostString());

	auto res = cli.Get(path, constructHeaders(mHeaders, mCookies));
	if (!res) {
		std::string url = constructHostString();
		url.append("/").append(path);
		throw HttplibRequestException("no response", url, 0, magic_enum::enum_name(res.error()).data());
	}
	if (res->status != 200) {
		auto host = constructHostString();
		std::string url(host);
		url.append("/").append(path);
		throw HttplibRequestException("status isn't 200 for GET", url, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

std::string HttpRequest::constructHostString()
{
	auto uri = furi::uri_split::from_uri(mUrl);
	// http | https
	std::string host(uri.scheme.data(), uri.scheme.size());
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
	host += "://" + std::string(uri.authority.data(), uri.authority.size());
	return host;
}
