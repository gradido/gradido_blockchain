#include "gradido_blockchain/http/GraphQLRequest.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/http/RequestExceptions.h"

using namespace rapidjson;


GraphQLRequest::GraphQLRequest(const Poco::URI& requestUri)
	: JsonRequest(requestUri)
{

}

Document GraphQLRequest::POST(const std::string& graphqlQuery)
{
	auto clientSession = createClientSession();

	Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, mRequestUri.getPath());

	request.setChunkedTransferEncoding(true);
	request.setContentType("application/json");
	request.add("Accept", "*/*");

	std::string responseString;
	try {
		std::ostream& request_stream = clientSession->sendRequest(request);
		request_stream << graphqlQuery;

		Poco::Net::HTTPResponse response;
		std::istream& response_stream = clientSession->receiveResponse(response);

		for (std::string line; std::getline(response_stream, line); ) {
			responseString += line + "\n";
		}
	}
	catch (Poco::Exception& ex) {
		throw PocoNetException(ex, mRequestUri, mRequestUri.getPath().data());
	}

	return parseResponse(responseString);
}

