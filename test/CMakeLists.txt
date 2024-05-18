cmake_minimum_required(VERSION 3.18.2)
project(GradidoBlockchainTest)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

IF(WIN32)
    include_directories(
        "../dependencies/protobuf/third_party/googletest/googletest/include"
    )
endif()

# collect source code files
FILE(GLOB TEST_MAIN "*.cpp" "*.h")
FILE(GLOB TEST_CRYPTO "crypto/*.cpp" "crypto/*.h")
FILE(GLOB TEST_LIB "lib/*.cpp" "lib/*.h")

SET(TEST_SRC ${TEST_MAIN} ${TEST_LIB} ${TEST_CRYPTO})

if(MSVC)
	# src
	source_group("lib" FILES ${TEST_LIB})
    source_group("crypto" FILES ${TEST_CRYPTO})
endif()

IF(WIN32)
    # if static lib build
    message("build shared libs: ${BUILD_SHARED_LIBS}")
ENDIF()

enable_testing()

#include(FetchContent)
#FetchContent_Declare(
#  googletest
#  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)

add_executable(GradidoBlockchainTest ${TEST_SRC})
target_link_libraries(GradidoBlockchainTest GradidoBlockchain libprotobuf gmock)
IF(WIN32)    
    find_library(ED25519_BIP32_RUST_C_LIB ed25519_bip32_c_interface.dll.lib PATHS 
		"${PROJECT_BINARY_DIR}/../dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/${CMAKE_BUILD_TYPE}"
		"${PROJECT_BINARY_DIR}/../dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/debug"
		"${PROJECT_BINARY_DIR}/../dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/release"
	)	
    
    IF(ED25519_BIP32_RUST_C_LIB STREQUAL "ED25519_BIP32_RUST_C_LIB-NOTFOUND" ) 
        message("!!!!!!\nplease build target ed25519_bip32_c_interface_target and call again `cmake`\n!!!!!!")
    ELSE()
        target_link_libraries(GradidoBlockchainTest ${ED25519_BIP32_RUST_C_LIB})
    ENDIF()
ELSE()
    target_link_libraries(GradidoBlockchainTest ed25519_bip32_c_interface gtest)
ENDIF()

IF(NOT BUILD_SHARED_LIBS) 
    IF(WIN32) 
        target_link_libraries(GradidoBlockchainTest ${CONAN_LIBS})
    ENDIF()
ENDIF()

include(GoogleTest)
gtest_discover_tests(GradidoBlockchainTest)