/*!
 *
 * \author: Dario Rekowski
 *
 * \date: 13.01.2021
 *
 * \brief: Class for Json Requests to php server
 *
*/

#include "JsonRequest.h"

#include <vector>

#ifndef __GRADIDO_LOGIN_SERVER_LIB_JSON_RPC_REQUEST_
#define __GRADIDO_LOGIN_SERVER_LIB_JSON_RPC_REQUEST_


enum GRADIDOBLOCKCHAIN_EXPORT JsonRPCErrorCodes : int
{
	JSON_RPC_ERROR_NONE = 0,
	JSON_RPC_ERROR_GRADIDO_NODE_ERROR = -10000,
	JSON_RPC_ERROR_UNKNOWN_GROUP = -10001,
	JSON_RPC_ERROR_NOT_IMPLEMENTED = -10002,
	JSON_RPC_ERROR_TRANSACTION_NOT_FOUND = -10003,
	JSON_RPC_ERROR_ADDRESS_NOT_FOUND = -10004,

	// default errors from json rpc standard: https://www.jsonrpc.org/specification
	// -32700 	Parse error 	Invalid JSON was received by the server.
	JSON_RPC_ERROR_PARSE_ERROR = -32700,
	// -32600 	Invalid Request The JSON sent is not a valid Request object.
	JSON_RPC_ERROR_INVALID_REQUEST = -32600,
	// -32601 	Method not found 	The method does not exist / is not available.
	JSON_RPC_ERROR_METHODE_NOT_FOUND = -32601,
	// -32602 	Invalid params 	Invalid method parameter(s).
	JSON_RPC_ERROR_INVALID_PARAMS = -32602,
	// -32603 	Internal error 	Internal JSON - RPC error.
	JSON_RPC_ERROR_INTERNAL_ERROR = -32603,
	// -32000 to -32099 	Server error 	Reserved for implementation-defined server-errors.

};

class GRADIDOBLOCKCHAIN_EXPORT JsonRPCRequest : public JsonRequest
{
public:
	JsonRPCRequest(const std::string& serverHost, int serverPort);
	JsonRPCRequest(const std::string& uri);
	~JsonRPCRequest();

	rapidjson::Document request(const char* methodName, rapidjson::Value& params);
	rapidjson::Document& batchRequest(std::vector<std::string> methods, rapidjson::Value& params);

protected:

};


#endif //__GRADIDO_LOGIN_SERVER_LIB_JSON_RPC_REQUEST_