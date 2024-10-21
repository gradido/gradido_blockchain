#ifndef __GRADIDO_BLOCKCHAIN_LIB_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_LIB_CONFIG_H

#include "fkYAML/node.hpp"
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
	// split key on .
	std::vector<std::string> splitKey(const std::string& key) const;
	inline const fkyaml::node* getNode(const std::string& key) const { return getNode(splitKey(key)); }
	const fkyaml::node* getNode(const std::vector<std::string>& path) const;
	
	fkyaml::node mRootNode;
};

#endif //__GRADIDO_BLOCKCHAIN_LIB_CONFIG_H