#ifndef __GRADIDO_BLOCKCHAIN_LIB_MINIZ_LIB_H
#define __GRADIDO_BLOCKCHAIN_LIB_MINIZ_LIB_H

#include "gradido_blockchain/memory/Block.h"
#include "gradido_blockchain/GradidoBlockchainException.h"

/*
* Wrapper for using miniz for compressing with memory::Block
*/

GRADIDOBLOCKCHAIN_EXPORT memory::Block compress(const memory::Block& plain);
GRADIDOBLOCKCHAIN_EXPORT memory::Block decompress(const memory::Block& compressed);

class GradidoMinizCompressException : GradidoBlockchainException
{
public:
    /*
    * \param contentPart: only put a part from content for privacy reasons
    */
    explicit GradidoMinizCompressException(
        const std::string& contentPart, 
        int minizStatus, 
        size_t contentSize,
        size_t bufferSize
    ) noexcept;
    std::string getFullString() const;
    rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

protected:
    std::string mContentPart;
    std::string mMinizStatusName;    
    size_t mContentSize;
    size_t mBufferSize;
};


class GradidoMinizDecompressException : GradidoBlockchainException
{
public:
    /*
    * \param contentPart: only put a part from content for privacy reasons
    */
    explicit GradidoMinizDecompressException(int minizStatus) noexcept;
    std::string getFullString() const;
    rapidjson::Value getDetails(rapidjson::Document::AllocatorType& alloc) const;

protected:
    std::string mMinizStatusName;
};


#endif //__GRADIDO_BLOCKCHAIN_LIB_MINIZ_LIB_H