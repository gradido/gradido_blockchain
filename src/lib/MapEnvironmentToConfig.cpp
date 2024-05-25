#include "gradido_blockchain/lib/MapEnvironmentToConfig.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <algorithm>
#include <cstdlib>

MapEnvironmentToConfig::MapEnvironmentToConfig(const std::string& fileName)
	: Config(fileName)
{
}

std::string MapEnvironmentToConfig::getString(const std::string& key, const std::string& defaultValue) const
{
	auto result = Config::getString(key, defaultValue);
	if (result == defaultValue) {
		auto envValue = std::getenv(mapKey(key).c_str());
		if (envValue) {
			result = envValue;
		}
	}
	return result;
}

int MapEnvironmentToConfig::getInt(const std::string& key, int defaultValue) const
{
	auto result = Config::getInt(key, defaultValue);
	if (result == defaultValue) {
		auto envValue = std::getenv(mapKey(key).c_str());
		if (envValue) {
			result = DataTypeConverter::strToInt(envValue);
		}
	}
	return result;
}

std::string MapEnvironmentToConfig::mapKey(const std::string& key) const
{
	std::string result = key;
	std::replace(result.begin(), result.end(), '.', '_'); // replace all '.' to '_'
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);
	return result;
}
