#ifndef __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H
#define __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H

#include <chrono>
#include "gradido_blockchain/data/EncryptedMemo.h"

#define VERSION_STRING "3.4"
const auto createdAt = std::chrono::system_clock::from_time_t(1609459200); //2021-01-01 00:00:00 UTC
const auto confirmedAt = std::chrono::system_clock::from_time_t(1609464130);
const auto targetDate = std::chrono::system_clock::from_time_t(1609459000);
const auto timeout = std::chrono::system_clock::from_time_t(1609465000);
const auto timeoutDuration = std::chrono::months(3);

const auto creationMemo = gradido::data::EncryptedMemo(
	gradido::data::MemoKeyType::PLAIN, 
	std::make_shared<memory::Block>("Deine erste Schoepfung ;)")
);
const auto transferMemo = gradido::data::EncryptedMemo(
	gradido::data::MemoKeyType::PLAIN,
	std::make_shared<memory::Block>("Ich teile mit dir")
);
const auto deferredTransferMemo = gradido::data::EncryptedMemo(
	gradido::data::MemoKeyType::PLAIN, 
	std::make_shared<memory::Block>("Link zum einloesen")
);
const auto aFilledMemo = gradido::data::EncryptedMemo(
	gradido::data::MemoKeyType::PLAIN, 
	std::make_shared<memory::Block>(std::string(451, 'a'))
); // fill with 451 x a
const auto hallMemo = gradido::data::EncryptedMemo(
	gradido::data::MemoKeyType::PLAIN,
	std::make_shared<memory::Block>("hall")
);
#endif //__GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H