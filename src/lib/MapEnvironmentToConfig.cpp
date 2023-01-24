#include "gradido_blockchain/lib/MapEnvironmentToConfig.h"
#include <algorithm>

MapEnvironmentToConfig::MapEnvironmentToConfig(Poco::Util::LayeredConfiguration& parent)
	: mParent(parent)
{
	add(&mParent);
}

std::string MapEnvironmentToConfig::getString(const std::string& key, const std::string& defaultValue) const
{
	auto result = mParent.getString(key, defaultValue);
	if (result == defaultValue) {
		result = mParent.getString(mapKey(key), defaultValue);
	}
	return result;
}

int MapEnvironmentToConfig::getInt(const std::string& key, int defaultValue) const
{
	auto result = mParent.getInt(key, defaultValue);
	if (result == defaultValue) {
		result = mParent.getInt(mapKey(key), defaultValue);
	}
	return result;
}

std::string MapEnvironmentToConfig::mapKey(const std::string& key) const
{
	std::string result = key;
	std::replace(result.begin(), result.end(), '.', '_'); // replace all '.' to '_'
	std::transform(result.begin(), result.end(), result.begin(), ::toupper);
	return "system.env." + result;
}
