#ifndef __GRADIDO_BLOCKCHAIN_MODEL_IOTA_TOPIC_H
#define __GRADIDO_BLOCKCHAIN_MODEL_IOTA_TOPIC_H

#include "gradido_blockchain/export.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include <string>

namespace iota {
    class GRADIDOBLOCKCHAIN_EXPORT Topic
	{
    public:
		Topic(const std::string& alias);
		Topic(const MemoryBin* rawIndex);

		inline const std::string& getBinString() const { return index; }
		inline std::string getHexString() const { return DataTypeConverter::binToHex(index); }

    protected:
        // member variables
        // index stored in string format, not hex
		std::string index;
	};
}

#endif //__GRADIDO_BLOCKCHAIN_MODEL_IOTA_TOPIC_H