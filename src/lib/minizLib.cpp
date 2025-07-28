#include "gradido_blockchain/lib/minizLib.h"
#include "gradido_blockchain/const.h"

#include "miniz.h"
#include "tinf/src/tinf.h"

using namespace rapidjson;
// 78DA is zlib Header on maximal kompression level without custom dictionary
static const uint8_t zlibHeaderData[] = { 0x78, 0xDA };

std::string getContentPart(const memory::Block& content) {
    if (content.size() < 6) {
        return content.copyAsString();
    }
    else {
        char buffer[6];
        memcpy(buffer, content.data(0), 3);
        memcpy(&buffer[3], content.data(content.size() - 4), 3);
        return std::string(buffer);
    }
}

memory::Block compress(const memory::Block& plain)
{
    if (plain.size() < GRADIDO_ENCRYPTED_MEMO_COMPRESSION_DEFAULT_SKIP_SIZE_BYTES) {
        return plain;
    }
    auto maxExpectedSize = compressBound(plain.size());
    size_t bufferSize = maxExpectedSize;
    // use if possible default buffer size to reduce memory allocation overhead
    if (GRADIDO_ENCRYPTED_MEMO_COMPRESSION_DEFAULT_BUFFER_BYTES >= maxExpectedSize) {
        bufferSize = GRADIDO_ENCRYPTED_MEMO_COMPRESSION_DEFAULT_BUFFER_BYTES;
    }
    memory::Block buffer(bufferSize);
    mz_ulong compressedSize = bufferSize;
    // status can be MZ_PARAM_ERROR, MZ_BUF_ERROR, MZ_STREAM_ERROR, MZ_OK
    auto status = compress2(buffer, &compressedSize, plain, plain.size(), 10);
    if (MZ_OK != status) {
        throw GradidoMinizCompressException(getContentPart(plain), status, plain.size(), bufferSize);
    }
    if (plain.size() <= compressedSize) {
        return plain;
    }
    return memory::Block(compressedSize, buffer);
}

memory::Block decompress(const memory::Block& compressed)
{
    if (0 != std::memcmp(compressed, zlibHeaderData, 2)) {
        return compressed;
    }
    size_t bufferSize = GRADIDO_ENCRYPTED_MEMO_COMPRESSION_DEFAULT_BUFFER_BYTES;
    memory::Block buffer(bufferSize);
    // status can be: 
    // TINF_OK
    // TINF_BUF_ERROR
    // TINF_DATA_ERROR
    unsigned int uncompressedSize = bufferSize;
    auto status = tinf_zlib_uncompress(buffer, &uncompressedSize, compressed, compressed.size());

    // if buffer is to small, try again with 2x Default buffer
    if (status == TINF_DATA_ERROR) {
        bufferSize *= 2;
        uncompressedSize = bufferSize;
        buffer = memory::Block(bufferSize);
        status = tinf_zlib_uncompress(buffer, &uncompressedSize, compressed, compressed.size());
    }
    if (TINF_OK != status) {
        throw GradidoTinfDecompressException(status);
    }
    return memory::Block(uncompressedSize, buffer);
}

const char* getMinizStatusName(int status) {
    switch (status) {
    case MZ_PARAM_ERROR: return "param error";
    case MZ_BUF_ERROR: return "buf error";
    case MZ_DATA_ERROR: return "data error";
    case MZ_STREAM_ERROR: return "stream error";
    case MZ_OK: return "ok";
    default: return "unknown";
    }
}

const char* getTinfStatusName(int status) {
    switch (status) {
    case TINF_DATA_ERROR: return "data error";
    case TINF_BUF_ERROR: return "buf error";
    case TINF_OK: return "ok";
    default: return "unknown";
    }
}

// exception
GradidoMinizCompressException::GradidoMinizCompressException(
    const std::string& contentPart,
    int minizStatus,
    size_t contentSize,
    size_t bufferSize
) noexcept
    : GradidoBlockchainException("error while compressing with miniz"), 
    mContentPart(contentPart), mMinizStatusName(getMinizStatusName(minizStatus)), mContentSize(contentSize), mBufferSize(bufferSize)
{

}

std::string GradidoMinizCompressException::getFullString() const
{
    std::string result = what();
    result += ", status: " + mMinizStatusName;
    result += ", content part: " + mContentPart;
    result += ", content size: " + std::to_string(mContentSize);
    result += ", buffer size: " + std::to_string(mBufferSize);
    return result;
}

rapidjson::Value GradidoMinizCompressException::getDetails(rapidjson::Document::AllocatorType& alloc) const
{ 
    Value result(kObjectType); 
    result.AddMember("what", Value(what(), alloc), alloc);
    result.AddMember("status", Value(mMinizStatusName.data(), mMinizStatusName.size(), alloc), alloc);
    result.AddMember("contentPart", Value(mContentPart.data(), mContentPart.size(), alloc), alloc);    
    result.AddMember("contentSize", mContentSize, alloc);
    result.AddMember("bufferSize", mBufferSize, alloc);
    return result;
}

GradidoTinfDecompressException::GradidoTinfDecompressException(int tinfStatus) noexcept
    : GradidoBlockchainException("error while uncompressing with tinf"), mTinfStatusName(getTinfStatusName(tinfStatus))
{

}

std::string GradidoTinfDecompressException::getFullString() const
{
    std::string result = what();
    result += ", status: " + mTinfStatusName;
    return result;
}

rapidjson::Value GradidoTinfDecompressException::getDetails(rapidjson::Document::AllocatorType& alloc) const
{
    Value result(kObjectType);
    result.AddMember("what", Value(what(), alloc), alloc);
    result.AddMember("status", Value(mTinfStatusName.data(), mTinfStatusName.size(), alloc), alloc);
    return result;
}