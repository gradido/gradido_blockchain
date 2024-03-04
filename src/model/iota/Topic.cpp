#include "gradido_blockchain/model/iota/Topic.h"

namespace iota
{
    Topic::Topic(const std::string& alias)
    {
        // is already hex
        // Q: https://stackoverflow.com/questions/8899069/how-to-find-if-a-given-string-conforms-to-hex-notation-eg-0x34ff-without-regex
        if (std::all_of(alias.begin(), alias.end(), ::isxdigit)) {
            index = std::move(*DataTypeConverter::hexToBinString(alias));
        }
        else {
            // is named
            index.reserve(alias.size() + 9);
            index = "GRADIDO.";
            index += alias;
        }
    }
    Topic::Topic(const MemoryBin* rawIndex)
        : index(std::string((const char*)rawIndex->data(), rawIndex->size()))
    {	
    }
} // namespace iota