#include "gradido_blockchain/http/GraphQLRequest.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/http/RequestExceptions.h"

#include "magic_enum/magic_enum.hpp"
#include "furi/furi.hpp"
#include "httplib.h"

using namespace rapidjson;


GraphQLRequest::GraphQLRequest(const std::string& requestUri)
	: JsonRequest(requestUri)
{

}

Document GraphQLRequest::POST(const std::string& graphqlQuery)
{
	auto uri = furi::uri_split::from_uri(mUrl);
	// http | https
	std::string host = uri.scheme.data();
	// host:port
	host += "://" + std::string(uri.authority.data());
	httplib::Client cli(host);

	// set Content-Type Header to application/json
	httplib::Headers headers = {
		{"Content-Type", "application/json"},
		{"Accept", "*/*"}
	};
	if (mCookies.size()) {
		std::ostringstream cookie_stream;
		for (const auto& [key, value] : mCookies) {
			cookie_stream << key << "=" << value << ";";
		}
		std::string cookie_header = cookie_stream.str();
		cookie_header.pop_back(); // Entferne das letzte Semikolon

		// Füge den Cookie-Header hinzu
		headers.emplace("Cookie", cookie_header);
	}
	
	auto res = cli.Post(uri.path.data(), headers, graphqlQuery, "application/json");
	if (res->status != 200) {
		throw HttplibRequestException("status isn't 200 for POST", mUrl, res->status, magic_enum::enum_name(res.error()).data());
	}
	return parseResponse(res->body);
}

