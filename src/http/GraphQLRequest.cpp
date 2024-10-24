#include "gradido_blockchain/http/GraphQLRequest.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "gradido_blockchain/GradidoBlockchainException.h"
#include "gradido_blockchain/http/RequestExceptions.h"


using namespace rapidjson;


GraphQLRequest::GraphQLRequest(const std::string& requestUri)
	: JsonRequest(requestUri)
{

}

Document GraphQLRequest::POST(const std::string& graphqlQuery)
{
	addHeader("Content-Type", "application/json");
	addHeader("Accept", "*/*");
	return parseResponse(HttpRequest::POST(graphqlQuery));
}

