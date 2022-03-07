#ifndef GRADIDO_BLOCKCHAIN_HTTP_GRAPHQL_REQUEST_H
#define GRADIDO_BLOCKCHAIN_HTTP_GRAPHQL_REQUEST_H

#include "gradido_blockchain/http/JsonRequest.h"

class GRADIDOBLOCKCHAIN_EXPORT GraphQLRequest : protected JsonRequest
{
public:
	GraphQLRequest(const Poco::URI& requestUri);

protected:	
};

#endif //GRADIDO_BLOCKCHAIN_HTTP_GRAPHQL_REQUEST_H