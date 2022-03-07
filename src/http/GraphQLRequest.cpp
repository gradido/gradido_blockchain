#include "gradido_blockchain/http/GraphQLRequest.h"

#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "gradido_blockchain/GradidoBlockchainException.h"

using namespace rapidjson;


GraphQLRequest::GraphQLRequest(const Poco::URI& requestUri)
	: JsonRequest(requestUri)
{

}

