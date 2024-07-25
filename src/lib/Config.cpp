#include "gradido_blockchain/lib/Config.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include <rapidjson/istreamwrapper.h>

#include <fstream>

using namespace rapidjson;

Config::Config(const std::string& fileName)
{
    std::ifstream fileStream(fileName);
    IStreamWrapper isw(fileStream);
    mJsonData.ParseStream(isw);
    if (mJsonData.HasParseError()) {
        throw RapidjsonParseErrorException("error parsing config", mJsonData.GetParseError(), mJsonData.GetErrorOffset())
            .setRawText(std::string((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>()));
    }
}

Config::~Config()
{

}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const
{
    if (mJsonData.HasMember(key.data()) && mJsonData[key.data()].IsString()) {
        return mJsonData[key.data()].GetString();
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const
{
    if (mJsonData.HasMember(key.data()) && mJsonData[key.data()].IsInt()) {
        return mJsonData[key.data()].GetInt();
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const
{
    if (mJsonData.HasMember(key.data()) && mJsonData[key.data()].IsBool()) {
        return mJsonData[key.data()].GetBool();
    }
    return defaultValue;
}