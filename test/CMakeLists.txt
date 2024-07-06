cmake_minimum_required(VERSION 3.18.2)
project(GradidoBlockchainTest)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 20)

IF(WIN32)
  find_package(gtest REQUIRED)
    include_directories(
        ${GTest_INCLUDE_DIRS}
    )
else()
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# collect source code files

FILE(GLOB TEST_BLOCKCHAIN "blockchain/*.cpp" "blockchain/*.h")
FILE(GLOB TEST_CRYPTO "crypto/*.cpp" "crypto/*.h")
FILE(GLOB TEST_INTERACTION "interaction/*.cpp" "interaction/*.h")
FILE(GLOB TEST_LIB "lib/*.cpp" "lib/*.h")
FILE(GLOB TEST_MAIN "*.cpp" "*.h")
FILE(GLOB TEST_MODEL "model/*.cpp" "model/*.h")
FILE(GLOB TEST_MODEL_PROTOPUF "model/protopuf/*.cpp" "model/protopuf/*.h")


SET(TEST_SRC 
  ${TEST_MAIN}
  ${TEST_LIB} 
  ${TEST_CRYPTO}
  #${TEST_MODEL}
  #${TEST_MODEL_PROTOPUF} 
  ${TEST_INTERACTION}
)

if(MSVC)
	# src
	source_group("lib" FILES ${TEST_LIB})
    source_group("crypto" FILES ${TEST_CRYPTO})
    source_group("model\\protopuf" FILES ${TEST_MODEL_PROTOPUF})
    source_group("model" FILES ${TEST_MODEL})
    source_group("interaction" FILES ${TEST_INTERACTION})    
endif()

IF(WIN32)
    # if static lib build
    message("build shared libs: ${BUILD_SHARED_LIBS}")
ENDIF()

enable_testing()


find_package(Sodium REQUIRED)

add_executable(GradidoBlockchainTest ${TEST_SRC})
target_link_libraries(GradidoBlockchainTest GradidoBlockchain sodium loguru::loguru) #libprotobuf
IF(WIN32)    
    find_library(ED25519_BIP32_RUST_C_LIB ed25519_bip32_c_interface.dll.lib PATHS 
		"${PROJECT_BINARY_DIR}/../dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/${CMAKE_BUILD_TYPE}"
		"${PROJECT_BINARY_DIR}/../dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/debug"
		"${PROJECT_BINARY_DIR}/../dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/release"
	)	
    
    IF(ED25519_BIP32_RUST_C_LIB STREQUAL "ED25519_BIP32_RUST_C_LIB-NOTFOUND" ) 
        message("!!!!!!\nplease build target ed25519_bip32_c_interface_target and call again `cmake`\n!!!!!!")
    ELSE()
        target_link_libraries(GradidoBlockchainTest ${ED25519_BIP32_RUST_C_LIB} ${GTest_LIBRARIES})
    ENDIF()
ELSE()
    target_link_libraries(GradidoBlockchainTest ed25519_bip32_c_interface gtest)
ENDIF()


IF(WIN32)
  add_custom_command(TARGET GradidoBlockchainTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:GradidoBlockchainTest> $<TARGET_FILE_DIR:GradidoBlockchainTest>
    COMMAND_EXPAND_LISTS
  )
ENDIF()


include(GoogleTest)
gtest_discover_tests(GradidoBlockchainTest)