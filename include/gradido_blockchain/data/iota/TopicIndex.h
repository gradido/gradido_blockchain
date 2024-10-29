#ifndef __GRADIDO_BLOCKCHAIN_MODEL_IOTA_TOPIC_H
#define __GRADIDO_BLOCKCHAIN_MODEL_IOTA_TOPIC_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <string>

namespace iota {
    class GRADIDOBLOCKCHAIN_EXPORT TopicIndex
	{
	public:
		TopicIndex(const std::string& alias);
		TopicIndex(std::string_view alias);
		TopicIndex(const memory::Block& rawIndex);

		inline std::string getBinString() const { return mIndex.copyAsString(); }
		inline std::string getHexString() const { return  mIndex.convertToHex(); }

	protected:
    // member variables
    // index stored in string format, not hex
		memory::Block mIndex;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_IOTA_TOPIC_H