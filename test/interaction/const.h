#ifndef __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H
#define __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H

#include <chrono>
#include "gradido_blockchain/data/EncryptedMemo.h"

#define VERSION_STRING "3.4"
const auto createdAt = std::chrono::system_clock::from_time_t(1609459200); //2021-01-01 00:00:00 UTC
const auto confirmedAt = std::chrono::system_clock::from_time_t(1609464130);
const auto targetDate = std::chrono::system_clock::from_time_t(1609459000);
const auto timeout = std::chrono::system_clock::from_time_t(1609465000);
const auto timeoutDuration = gradido::data::DurationSeconds(std::chrono::months(3));

const auto creationMemoString = "Deine erste Schoepfung ;)";
const auto transferMemoString = "Ich teile mit dir";
const auto deferredTransferMemoString = "Link zum einloesen";
const auto aFilledMemoString = std::string(451, 'a');
const auto hallMemoString = "hall";
const auto completeTransactionMemoString = "Danke fuer dein Sein!";
#endif //__GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H