#ifndef __GRADIDO_BLOCKCHAIN_LIB_MAP_ENVIRONMENT_TO_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_LIB_MAP_ENVIRONMENT_TO_CONFIG_H

#include "Config.h"
#include "gradido_blockchain/export.h"

/*!
  @author einhornimmond
  @date 06.09.2022
  @brief Use Environment Variables if config option wasn't found in config file
 
@startuml
start
:read value for key from config;
if (value == defaultValue?) then (yes)
	: map key to env key;
	: and read from env;
else (no)
	: return value;
@enduml
*/

class GRADIDOBLOCKCHAIN_EXPORT MapEnvironmentToConfig : public Config
{
public:
	MapEnvironmentToConfig(const std::string& fileName);
	~MapEnvironmentToConfig() {}
	//! call getString from Config, and if that return defaultValue, 
	//! call mapKey to translate the config key to an environment key and call again getString but with the mapped key
	std::string getString(const std::string& key, const std::string& defaultValue) const;

	//! call getInt from Config, and if that return defaultValue, 
	//! call mapKey to translate the config key to an environment key and call again getInt but with the mapped key
	int getInt(const std::string& key, int defaultValue) const;

	bool getBool(const std::string& key, bool defaultValue) const;

protected:
	//! replace all . with _ and make all letters uppercase
	//! prepends system.env
	//! translate for example db.sqlite to system.env.DB_SQLITE
	//! this try to read the env variable DB_SQLITE
	std::string mapKey(const std::string& key) const;
};
#endif //__GRADIDO_BLOCKCHAIN_LIB_MAP_ENVIRONMENT_TO_CONFIG_H