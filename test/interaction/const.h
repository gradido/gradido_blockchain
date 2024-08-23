#ifndef __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H
#define __GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H

#include <chrono>

#define VERSION_STRING "3.3"
const auto createdAt = std::chrono::system_clock::from_time_t(1609459200); //2021-01-01 00:00:00 UTC
const auto confirmedAt = std::chrono::system_clock::from_time_t(1609464130);
const auto targetDate = std::chrono::system_clock::from_time_t(1609459000);
const auto timeout = std::chrono::system_clock::from_time_t(1609465000);


#endif //__GRADIDO_BLOCKCHAIN_TEST_INTERACTION_CONST_H