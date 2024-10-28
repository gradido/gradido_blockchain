#include "gradido_blockchain/data/iota/TopicIndex.h"

#include "loguru/loguru.hpp"

#include <algorithm>


namespace iota
{
    TopicIndex::TopicIndex(const std::string& alias)
        : TopicIndex(std::string_view(alias))
    {
    }
    TopicIndex::TopicIndex(std::string_view alias)
        : mIndex(0)
    {
        // is already hex
        // Q: https://stackoverflow.com/questions/8899069/how-to-find-if-a-given-string-conforms-to-hex-notation-eg-0x34ff-without-regex
        if (std::all_of(alias.begin(), alias.end(), ::isxdigit)) {
            mIndex = memory::Block::fromHex(alias);
        }
        else {
            // is named
            std::string indexString;
            indexString.reserve(alias.size() + 9);
            indexString = "GRADIDO.";
            indexString += alias;
            mIndex = memory::Block(indexString);
        }
    }
    TopicIndex::TopicIndex(const memory::Block& rawIndex)
        : mIndex(rawIndex)
    {	
    }
} // namespace iota