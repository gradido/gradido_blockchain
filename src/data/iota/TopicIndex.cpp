#include "gradido_blockchain/data/iota/TopicIndex.h"
#include <algorithm>

namespace iota
{
    TopicIndex::TopicIndex(const std::string& alias)
        : TopicIndex(std::string_view(alias))
    {
    }
    TopicIndex::TopicIndex(std::string_view alias)
    {

        // is already hex
        // Q: https://stackoverflow.com/questions/8899069/how-to-find-if-a-given-string-conforms-to-hex-notation-eg-0x34ff-without-regex
        if (std::all_of(alias.begin(), alias.end(), ::isxdigit)) {
            index = alias;
        }
        else {
            // is named
            index.reserve(alias.size() + 9);
            index = "GRADIDO.";
            index += alias;
        }
    }
    TopicIndex::TopicIndex(const memory::Block& rawIndex)
        : index(rawIndex.copyAsString())
    {	
    }
} // namespace iota