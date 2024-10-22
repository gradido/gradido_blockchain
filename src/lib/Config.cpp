#include "gradido_blockchain/lib/Config.h"
#include "gradido_blockchain/GradidoBlockchainException.h"
#include <rapidjson/istreamwrapper.h>

#include <fstream>
#include <filesystem>

using namespace rapidjson;

Config::Config(const std::string& fileName)
{
    if (!fileName.empty() && std::filesystem::is_regular_file(fileName)) {
        std::ifstream fileStream(fileName);
        mRootNode = fkyaml::node::deserialize(fileStream);
    }
}

Config::~Config()
{

}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const
{
    auto node = getNode(key);
    if (!node) {
        return defaultValue;
    }
    return node->get_value<std::string>();
}

int Config::getInt(const std::string& key, int defaultValue) const
{
    auto node = getNode(key);
    if (!node) {
        return defaultValue;
    }
    return node->get_value<int>();
}

bool Config::getBool(const std::string& key, bool defaultValue) const
{
    auto node = getNode(key);
    if (!node) {
        return defaultValue;
    }
    return node->get_value<bool>();
}

std::vector<std::string> Config::splitKey(const std::string& key) const
{
    std::vector<std::string> tokens;
    std::istringstream stream(key);
    std::string token;

    while (std::getline(stream, token, '.')) {
        tokens.push_back(token);
    }

    return tokens;
}

const fkyaml::node* Config::getNode(const std::vector<std::string>& path) const
{
    const fkyaml::node* current = &mRootNode;
    for (const auto& key : path) {
        if ((*current)[key].is_null()) {
            return nullptr;
        }
        current = &(*current)[key];
    }
    return current;
}