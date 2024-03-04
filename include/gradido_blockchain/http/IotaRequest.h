#ifndef __GRADIDO_LOGIN_SERVER_IOTA_REQUEST
#define __GRADIDO_LOGIN_SERVER_IOTA_REQUEST

#include "JsonRequest.h"

#include "rapidjson/document.h"

#include "gradido_blockchain/MemoryManager.h"
#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/model/iota/NodeInfo.h"
#include "gradido_blockchain/model/iota/Topic.h"

#include "Poco/Logger.h"

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