#ifndef __GRADIDO_LOGIN_SERVER_IOTA_REQUEST
#define __GRADIDO_LOGIN_SERVER_IOTA_REQUEST

#include "JsonRequest.h"

#include "rapidjson/document.h"

#include "gradido_blockchain/MemoryManager.h"

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
}

/*!
	
	@author einhornimmond

	@brief different iota calls

	iota api endpoints:
	https://docs.rs/iota-client/1.0.1/iota_client/client/struct.Client.html
*/

// 

class GRADIDOBLOCKCHAIN_EXPORT IotaRequest : public JsonRequest
{
public:
	IotaRequest(const std::string& serverHost, int serverPort, const std::string& urlPath);
	~IotaRequest();	

	//! \param indexHex iota index in hex format, for gradido transaction: GRADIDO.<groupAlias>
	//! \return messageId as hex string
	std::string sendMessage(const std::string& indexHex, const std::string& messageHex);
	rapidjson::Document getMessageJson(const std::string& messageIdHex);
	std::pair<std::string, std::unique_ptr<std::string>> getIndexiationMessageDataIndex(const std::string& messageIdHex);
	//! use metadata call to check if it is referenced by a milestone
	//! \return return 0 if not, else return milestone id
	uint32_t getMessageMilestoneId(const std::string& messageIdHex);
	//! caller must release MemoryBins
	std::vector<MemoryBin*> findByIndex(const std::string& index);
	MemoryBin* getMilestoneByIndex(int32_t milestoneIndex);
	uint64_t getMilestoneTimestamp(int32_t milestoneIndex);
	iota::NodeInfo getNodeInfo();

protected:
	// Iota get parent message ids for own message
	std::vector<std::string> getTips();

};

#endif //__GRADIDO_LOGIN_SERVER_IOTA_REQUEST