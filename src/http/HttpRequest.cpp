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

// this is to prevent crashes, because at least with C++17 and C++20 httplib crashes with calling httplib::Client deconstructor
// so we create a new client for each new host and keep them forever and don't delete them even on program exit
// TODO: Fix bug in httplib which leads to this crash
struct FakeDeleter
{
    void operator()(httplib::Client* client) const
    {
			#ifdef WIN32
				delete client;
			#else
			// leak memory on linux to prevent crashes on exit
			#endif
    }
};
static std::multimap<std::string, std::shared_ptr<httplib::Client>> mHttpClients;
static std::mutex mClientsMutex;

static std::shared_ptr<httplib::Client> getClientForHost(const std::string& host, bool isSSL = false)
{
	std::lock_guard _lock(mClientsMutex);
	auto range = mHttpClients.equal_range(host);
	for (auto it = range.first; it != range.second; ++it) {
		if (it->second.use_count() == 1) {
			return it->second;
		}
	}

	auto httpClient = std::shared_ptr<httplib::Client>(new httplib::Client(host), FakeDeleter());
	httpClient->set_keep_alive(true);
	LOG_F(INFO, "created new HTTP%s client for host: %s", isSSL ? "S" : "", host.data());
	mHttpClients.insert({ host, httpClient });
	return httpClient;
}

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
	: mUrl(url), mIsSSL(false)
{
	auto uri = furi::uri_split::from_uri(mUrl);
	if (uri.scheme != "http" && uri.scheme != "https") {
		throw RequestException("cannot find scheme (http|https) in url", url);
	}
	if (uri.scheme == "https") {
		mIsSSL = true;
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
			mIsSSL = true;
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
	auto cli = getClientForHost(constructHostString(), mIsSSL);
	auto uri = furi::uri_split::from_uri(mUrl);

	std::string finalPath;
	if (!path) {
		finalPath = uri.path;
	}
	else {
		finalPath = path;
	}

	auto res = cli->Post(finalPath.data(), constructHeaders(mHeaders, mCookies), body, contentType);
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
	auto cli = getClientForHost(constructHostString(), mIsSSL);
	auto uri = furi::uri_split::from_uri(mUrl);
	std::string pathAndQuery("/");

	if (!path) {
		if (uri.path.size()) {
			if(uri.path.data()[0] != '/') pathAndQuery += "/";
			pathAndQuery = std::string(uri.path.data(), uri.path.size());
		}
	}
	else {
		pathAndQuery = path;
	}
	if (query.empty()) {
		if (uri.query.size()) {
			pathAndQuery += "?" + std::string(uri.query.data(), uri.query.size());
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
	auto res = cli->Get(pathAndQuery, constructHeaders(mHeaders, mCookies));
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
	std::string host = constructHostString();
	auto cli = getClientForHost(host, mIsSSL);

	auto res = cli->Get(path, constructHeaders(mHeaders, mCookies));
	if (!res) {
		std::string url = host;
		url.append("/").append(path);
		throw HttplibRequestException("no response", url, 0, magic_enum::enum_name(res.error()).data());
	}
	if (res->status != 200) {
		std::string url = host;
		url.append("/").append(path);
		throw HttplibRequestException("status isn't 200 for GET", url, res->status, magic_enum::enum_name(res.error()).data());
	}
	return res->body;
}

std::string HttpRequest::constructHostString()
{
	auto uri = furi::uri_split::from_uri(mUrl);
	LOG_F(
		INFO, "uri split: scheme: %s, authority: %s, path: %s, query: %s, fragment: %s",
		uri.scheme.data(), uri.authority.data(), uri.path.data(), uri.query.data(), uri.fragment.data()
	);
	// http | https
	std::string host(uri.scheme.data(), uri.scheme.size());
	if (host.empty()) {
		host += "http";
	}
#ifndef USE_HTTPS
	if (host == "https") {
		mIsSSL = false;
		LOG_F(WARNING, "try to make Https Request but cpp-httplib was included without OpenSSL-Support, changed to http");
	}
#endif
	// host:port
	return std::string(uri.authority.data(), uri.authority.size());
}
