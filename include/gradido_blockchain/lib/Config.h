#ifndef __GRADIDO_BLOCKCHAIN_LIB_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_LIB_CONFIG_H

#include "rapidjson/document.h"
#include <string>

class Config 
{
public: 
	Config(const std::string& fileName);
	virtual ~Config();

	std::string getString(const std::string& key, const std::string& defaultValue) const;
	int getInt(const std::string& key, int defaultValue) const;
	bool getBool(const std::string& key, bool defaultValue) const;

protected:
	rapidjson::Document mJsonData;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_CONFIG_H