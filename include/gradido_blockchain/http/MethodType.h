#ifndef __GRADIDO_BLOCKCHAIN_HTTP_METHOD_TYPE_H
#define __GRADIDO_BLOCKCHAIN_HTTP_METHOD_TYPE_H

#include <typeinfo>

enum class MethodType : uint8_t {
	GET,
	POST,
	PUT,
	DEL, // cannot use DELETE because it is reserved keyword in c++
	OPTIONS
};

#endif //__GRADIDO_BLOCKCHAIN_HTTP_METHOD_TYPE_H