#ifndef __GRADIDO_LOGIN_SERVER_IOTA_REQUEST
#define __GRADIDO_LOGIN_SERVER_IOTA_REQUEST

#include "JsonRequest.h"

#include "rapidjson/document.h"

#include "gradido_blockchain/lib/DataTypeConverter.h"
#include "gradido_blockchain/data/iota/NodeInfo.h"
#include "gradido_blockchain/data/iota/TopicIndex.h"

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
	std::string sendMessage(const iota::TopicIndex& index, const std::string& messageHex);
	rapidjson::Document getMessageJson(const std::string& messageIdHex);
	std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>> getIndexiationMessageDataIndex(const std::string& messageIdHex);
	//! use metadata call to check if it is referenced by a milestone
	//! \return return 0 if not, else return milestone id
	uint32_t getMessageMilestoneId(const std::string& messageIdHex);
	std::vector<memory::Block> findByIndex(const iota::TopicIndex& index);
	memory::Block getMilestoneByIndex(int32_t milestoneIndex);
	uint64_t getMilestoneTimestamp(int32_t milestoneIndex);
	iota::NodeInfo getNodeInfo();

	//! \brief use inside of catch clause by calling one of the iota requests
	//! \param terminate if set to true program will be terminated!
	static void defaultExceptionHandler(bool terminate = true);

protected:
	// Iota get parent message ids for own message
	std::vector<std::string> getTips();
#ifdef ENABLE_IOTA_RUST_CLIENT
	std::string sendMessageViaRustIotaClient(const std::string& index, const std::string& message);
#endif
	std::string_view extractPathFromUrl();
	std::string buildFullPath(const std::string& first, const std::string& second = "", const std::string& third = "");

};

#endif //__GRADIDO_LOGIN_SERVER_IOTA_REQUEST