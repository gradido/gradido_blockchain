cmake_minimum_required(VERSION 3.18.2)
cmake_policy(SET CMP0135 NEW)
project(GradidoBlockchainTest)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# collect source code files

FILE(GLOB TEST_BLOCKCHAIN "blockchain/*.cpp" "blockchain/*.h")
FILE(GLOB TEST_CRYPTO "crypto/*.cpp" "crypto/*.h")
FILE(GLOB TEST_INTERACTION "interaction/*.cpp" "interaction/*.h")
FILE(GLOB TEST_LIB "lib/*.cpp" "lib/*.h")
FILE(GLOB TEST_MAIN "*.cpp" "*.h")
FILE(GLOB TEST_DATA "data/*.cpp" "data/*.h")
FILE(GLOB TEST_DATA_HIERO "data/hiero/*.cpp" "data/hiero/*.h")
FILE(GLOB TEST_SERIALIZATION "serialization/*.cpp" "serialization/*.h")


SET(TEST_SRC 
  ${TEST_BLOCKCHAIN}
  ${TEST_MAIN}
  ${TEST_LIB} 
  ${TEST_CRYPTO}
  ${TEST_DATA}
  ${TEST_DATA_HIERO}
  ${TEST_INTERACTION}
  ${TEST_SERIALIZATION}
)

if(MSVC)
	# src
	source_group("lib" FILES ${TEST_LIB})
  source_group("blockchain" FILES ${TEST_BLOCKCHAIN})
  source_group("crypto" FILES ${TEST_CRYPTO})
  source_group("data\\hiero" FILES ${TEST_DATA_HIERO})
  source_group("data" FILES ${TEST_DATA})
  source_group("interaction" FILES ${TEST_INTERACTION})    
  source_group("serialization" FILES ${TEST_SERIALIZATION})    
endif()

IF(WIN32)
    # if static lib build
    message("build shared libs: ${BUILD_SHARED_LIBS}")
ENDIF()

enable_testing()


#find_package(Sodium REQUIRED)

add_executable(GradidoBlockchainTest ${TEST_SRC})
target_link_libraries(GradidoBlockchainTest GradidoBlockchain sodium loguru::loguru) #libprotobuf
target_link_libraries(GradidoBlockchainTest gtest)
if(openssl_FOUND) 
	target_link_libraries(GradidoBlockchainTest openssl::openssl)
endif()

include_directories(
	"../dependencies/date/include"
  "${googletest_SOURCE_DIR}/googletest/include"
)

IF(WIN32)
  add_custom_command(TARGET GradidoBlockchainTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:GradidoBlockchainTest> $<TARGET_FILE_DIR:GradidoBlockchainTest>
    COMMAND_EXPAND_LISTS
  )
ENDIF()

include(GoogleTest)
if(NOT CMAKE_CROSSCOMPILING OR (DEFINED CMAKE_CROSSCOMPILING_EMULATOR AND CMAKE_CROSSCOMPILING_EMULATOR))
  message(STATUS "Enable GradidoBlockchainTest tests")
  gtest_discover_tests(GradidoBlockchainTest)
endif()