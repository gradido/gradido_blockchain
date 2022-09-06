#ifndef __GRADIDO_BLOCKCHAIN_LIB_MAP_ENVIRONMENT_TO_CONFIG_H
#define __GRADIDO_BLOCKCHAIN_LIB_MAP_ENVIRONMENT_TO_CONFIG_H

#include "Poco/Util/LayeredConfiguration.h"
#include "gradido_blockchain/export.h"

/*!
  @author einhornimmond
  @date 06.09.2022
  @brief Use Environment Variables if config option wasn't found in properties file
 
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

class GRADIDOBLOCKCHAIN_EXPORT MapEnvironmentToConfig : public Poco::Util::LayeredConfiguration
{
public:
	MapEnvironmentToConfig(Poco::Util::LayeredConfiguration& parent);
	//! call getString from Poco::Util::LayeredConfiguration given on construction, and if that return defaultValue, 
	//! call mapKey to translate the config key to an environment key and call again Poco::Util::LayeredConfiguration::getString but with the mapped key
	std::string getString(const std::string& key, const std::string& defaultValue) const;

	//! call getInt from Poco::Util::LayeredConfiguration given on construction, and if that return defaultValue, 
	//! call mapKey to translate the config key to an environment key and call again Poco::Util::LayeredConfiguration::getInt but with the mapped key
	int getInt(const std::string& key, int defaultValue) const;

protected:
	//! replace all . with _ and make all letters uppercase
	//! prepends system.env
	//! translate for example db.sqlite to system.env.DB_SQLITE
	//! this try to read the env variable DB_SQLITE
	std::string mapKey(const std::string& key) const;

	Poco::Util::LayeredConfiguration& mParent;
};
#endif //__GRADIDO_BLOCKCHAIN_LIB_MAP_ENVIRONMENT_TO_CONFIG_H