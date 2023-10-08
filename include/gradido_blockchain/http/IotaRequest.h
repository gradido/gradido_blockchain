#ifndef __GRADIDO_LOGIN_SERVER_IOTA_REQUEST
#define __GRADIDO_LOGIN_SERVER_IOTA_REQUEST

#include "JsonRequest.h"

#include "rapidjson/document.h"

#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"

#include "Poco/Logger.h"

namespace iota {
	struct NodeInfo
	{
		std::string version;
		bool isHealthy;
		float messagesPerSecond;
		int32_t lastMilestoneIndex;
		int64_t lastMilestoneTimestamp;
		int32_t confirmedMilestoneIndex;
	};


	struct Topic
	{
		Topic(const std::string& alias)
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
		Topic(const MemoryBin* rawIndex)
			: index(std::string((const char*)rawIndex->data(), rawIndex->size()))
		{	
		}
		inline const std::string& getBinString() const { return index; }
		inline std::string getHexString() const { return DataTypeConverter::binToHex(index); }
		std::string index;
	};
}

/*!		
	@author einhornimmond

	@brief different iota calls

	iota api endpoints:
	https://docs.rs/iota-client/1.0.1/iota_client/client/struct.Client.html
*/
class GRADIDOBLOCKCHAIN_EXPORT IotaRequest : public JsonRequest
{
public:
	IotaRequest(const std::string& serverHost, int serverPort, const std::string& urlPath);
	~IotaRequest();	

	//! \param indexHex iota index in hex format, for gradido transaction: GRADIDO.<groupAlias>
	//! \return messageId as hex string
	std::string sendMessage(const iota::Topic& index, const std::string& messageHex);
	rapidjson::Document getMessageJson(const std::string& messageIdHex);
	std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>> getIndexiationMessageDataIndex(const std::string& messageIdHex);
	//! use metadata call to check if it is referenced by a milestone
	//! \return return 0 if not, else return milestone id
	uint32_t getMessageMilestoneId(const std::string& messageIdHex);
	//! caller must release MemoryBins
	std::vector<MemoryBin*> findByIndex(const iota::Topic& index);
	MemoryBin* getMilestoneByIndex(int32_t milestoneIndex);
	uint64_t getMilestoneTimestamp(int32_t milestoneIndex);
	iota::NodeInfo getNodeInfo();

	//! \brief use inside of catch clause by calling one of the iota requests
	//! \param terminate if set to true program will be terminated!
	static void defaultExceptionHandler(Poco::Logger& errorLog, bool terminate = true);

protected:
	// Iota get parent message ids for own message
	std::vector<std::string> getTips();

	std::string sendMessageViaRustIotaClient(const std::string& index, const std::string& message);

};

#endif //__GRADIDO_LOGIN_SERVER_IOTA_REQUEST