#ifndef GRADIDO_BLOCKCHAIN_HTTP_GRAPHQL_REQUEST_H
#define GRADIDO_BLOCKCHAIN_HTTP_GRAPHQL_REQUEST_H

#include "gradido_blockchain/http/JsonRequest.h"

class GRADIDOBLOCKCHAIN_EXPORT GraphQLRequest : public JsonRequest
{
public:
	GraphQLRequest(const std::string& requestUri);

	rapidjson::Document POST(const std::string& graphqlQuery);

protected:	
};

#endif //GRADIDO_BLOCKCHAIN_HTTP_GRAPHQL_REQUEST_H