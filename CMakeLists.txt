# Setup inspired from https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.18.2)
cmake_policy(VERSION 3.18.2)
project(GradidoBlockchain VERSION 2.0.0)

option(ENABLE_IOTA_RUST_CLIENT "switch for include rust iota client into build, increase build time and size a lot" OFF)
option(ENABLE_HTTP "switch for include http based classes into build, increase build time and size" OFF)
option(ENABLE_TEST "switch for include test into build config" OFF)

IF(WIN32)
	set(CMAKE_CXX_FLAGS         "/MP /EHsc /std:c++20")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	set(BUILD_SHARED_LIBS ON)
ELSE()
	SET(CMAKE_CXX_FLAGS "-std=c++20" )# -fPIC" )
ENDIF()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DNOMINMAX)
if(ENABLE_IOTA_RUST_CLIENT)
    add_definitions(-DENABLE_IOTA_RUST_CLIENT)
	set(ENABLE_HTTP ON)
endif()
if(ENABLE_HTTP)
    add_definitions(-DENABLE_HTTP)
endif()
set(INSTALL_BINDIR "bin")
set(INSTALL_PLUGINDIR "bin")

if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
    NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED GradidoBlockchain_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${GradidoBlockchain_SHARED_LIBS}")
endif()

############################## find dependencies ###################################
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Fetch Content Module
include(FetchContent)

# Update the commit to point to whatever libsodium-cmake-commit you want to target.
FetchContent_Declare(Sodium
    GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
    GIT_TAG e5b985ad0dd235d8c4307ea3a385b45e76c74c6a # HEAD, last updated at 2025-04-13
)
set(SODIUM_DISABLE_TESTS ON)
FetchContent_MakeAvailable(Sodium)

IF(WIN32)
	#set(sodium_DIR "C:/libsodium" CACHE FILEPATH "sodium install directory")
	#include(FindSodium)
  
  #find_package(Sodium REQUIRED)
	#include_directories(${Sodium_INCLUDE_DIR})
	if(ENABLE_HTTP)
		if(TARGET ssl AND TARGET crypto)
			message("[GradidoBlockchain] ssl found, enable use of https")
			add_compile_definitions(USE_HTTPS)
		else()
			find_package(OpenSSL)
			if(openssl_FOUND)
				message("[GradidoBlockchain] openSSL found, enable use of https")
				include_directories(${OPENSSL_INCLUDE_DIR})
				add_compile_definitions(USE_HTTPS)
			else()
				message("[GradidoBlockchain] openSSL not found, disable use of https")
				add_compile_definitions(DISABLE_HTTPS)
			endif()
		endif()
	endif()
ENDIF()
# find_package(Sodium REQUIRED)

############################## bind source files ###################################
FILE(GLOB CRYPTO "include/gradido_blockchain/crypto/*.h" "src/crypto/*.cpp")
FILE(GLOB DATA "include/gradido_blockchain/data/*.h" "src/data/*.cpp")
FILE(GLOB DATA_IOTA "include/gradido_blockchain/data/iota/*.h" "src/data/iota/*.cpp")
FILE(GLOB DATA_HIERO "include/gradido_blockchain/data/hiero/*.h" "src/data/hiero/*.cpp")
FILE(GLOB HTTP "include/gradido_blockchain/http/*.h" "src/http/*.cpp")
FILE(GLOB INTERACTION "include/gradido_blockchain/interaction/*.h" "src/interaction/*.cpp")

FILE(GLOB LIB_SRC "include/gradido_blockchain/lib/*.h" "src/lib/*.cpp")
FILE(GLOB MAIN "src/*.cpp" "src/*.c"  "include/gradido_blockchain/*.h")
FILE(GLOB MEMORY "include/gradido_blockchain/memory/*.h" "src/memory/*.cpp")
FILE(GLOB PROTO_GRADIDO "build/proto/gradido/*.cc" "build/proto/gradido/*.h")
FILE(GLOB TINF "dependencies/tinf/src/*.c" "dependencies/tinf/src/*.h")
FIlE(GLOB THREADING "include/gradido_blockchain/threading/*.h" "src/threading/*.cpp")
FILE(GLOB BLOCKCHAIN "include/gradido_blockchain/blockchain/*.h" "src/blockchain/*.cpp")


FILE(GLOB INTERACTION_ADVANCED_BLOCKCHAIN_FILTER 
	"include/gradido_blockchain/interaction/advancedBlockchainFilter/*.h" 
	"src/interaction/advancedBlockchainFilter/*.cpp"
)
FILE(GLOB INTERACTION_CALCULATE_ACCOUNT_BALANCE 
	"include/gradido_blockchain/interaction/calculateAccountBalance/*.h" 
	"src/interaction/calculateAccountBalance/*.cpp"
)
FILE(GLOB INTERACTION_CALCULATE_CREATION_SUM
	"include/gradido_blockchain/interaction/calculateCreationSum/*.h" 
	"src/interaction/calculateCreationSum/*.cpp"
)
FILE(GLOB INTERACTION_CONFIRM_TRANSACTION 
	"include/gradido_blockchain/interaction/confirmTransaction/*.h" 
	"src/interaction/confirmTransaction/*.cpp"
)
FILE(GLOB INTERACTION_CREATE_TRANSACTION_BY_EVENT
	"include/gradido_blockchain/interaction/createTransactionByEvent/*.h" 
	"src/interaction/createTransactionByEvent/*.cpp"
)
FILE(GLOB INTERACTION_DESERIALIZE 
	"include/gradido_blockchain/interaction/deserialize/*.h" 
	"src/interaction/deserialize/*.cpp"
)
FILE(GLOB INTERACTION_SERIALIZE 
	"include/gradido_blockchain/interaction/serialize/*.h" 
	"src/interaction/serialize/*.cpp"
)
FILE(GLOB INTERACTION_TO_JSON 
	"include/gradido_blockchain/interaction/toJson/*.h" 
	"src/interaction/toJson/*.cpp"
)
FILE(GLOB INTERACTION_VALIDATE 
	"include/gradido_blockchain/interaction/validate/*.h"
	"src/interaction/validate/*.cpp"
)

SET(GRADIDO_BLOCKCHAIN_LOCAL_SRCS 
	${BLOCKCHAIN}
	${CRYPTO}
	${DATA} ${DATA_IOTA} ${DATA_HIERO}
	${INTERACTION}
	${LIB_SRC} 
	${MAIN}
	${MEMORY}
	${MODEL_IOTA} 
	${TINF} 
	${THREADING}
	${INTERACTION_ADVANCED_BLOCKCHAIN_FILTER}
	${INTERACTION_CALCULATE_ACCOUNT_BALANCE}
	${INTERACTION_CALCULATE_CREATION_SUM}
	${INTERACTION_CONFIRM_TRANSACTION}
	${INTERACTION_CREATE_TRANSACTION_BY_EVENT}
	${INTERACTION_DESERIALIZE}
	${INTERACTION_SERIALIZE}
	${INTERACTION_TO_JSON}
	${INTERACTION_VALIDATE}
)

if(ENABLE_HTTP)
	SET(GRADIDO_BLOCKCHAIN_LOCAL_SRCS ${GRADIDO_BLOCKCHAIN_LOCAL_SRCS} ${HTTP})
endif()

if(MSVC)
	# src
	source_group("blockchain" FILES ${BLOCKCHAIN})
	source_group("crypto" FILES ${CRYPTO})
	source_group("data\\iota" FILES ${DATA_IOTA})
	source_group("data\\hiero" FILES ${DATA_HIERO})
	source_group("data" FILES ${DATA})
	if(ENABLE_HTTP)
		source_group("http" FILES ${HTTP})
	endif()
	source_group("interaction" FILES ${INTERACTION})
	source_group("lib" FILES ${LIB_SRC})	
	source_group("memory" FILES ${MEMORY})
	source_group("tinf" FILES ${TINF})
	source_group("threading" FILES ${THREADING})	
	source_group("interaction\\advancedBlockchainFilter" FILES ${INTERACTION_ADVANCED_BLOCKCHAIN_FILTER})
	source_group("interaction\\calculateAccountBalance" FILES ${INTERACTION_CALCULATE_ACCOUNT_BALANCE})
	source_group("interaction\\calculateCreationSum" FILES ${INTERACTION_CALCULATE_CREATION_SUM})
	source_group("interaction\\confirmTransaction" FILES ${INTERACTION_CONFIRM_TRANSACTION})
	source_group("interaction\\createTransactionByEvent" FILES ${INTERACTION_CREATE_TRANSACTION_BY_EVENT})
	source_group("interaction\\deserialize" FILES ${INTERACTION_DESERIALIZE})
	source_group("interaction\\serialize" FILES ${INTERACTION_SERIALIZE})
	source_group("interaction\\toJson" FILES ${INTERACTION_TO_JSON})
	source_group("interaction\\validate" FILES ${INTERACTION_VALIDATE})
endif()

include_directories(
	"dependencies"
	"dependencies/date/include"
	"dependencies/fkYAML/single_include"
	"dependencies/furi/code"
	"dependencies/magic_enum/include"
	"dependencies/protopuf/include"
	"dependencies/rapidjson/include"
	"dependencies/tinf/src" 
	"dependencies/miniz"
	"build/dependencies/miniz"
	"include"
)

if(ENABLE_HTTP)
	include_directories("dependencies/cpp-httplib")
endif()

if(ENABLE_IOTA_RUST_CLIENT)
  include_directories("iota_rust_clib/include")
ENDIF()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
IF(WIN32)
	message("[GradidoBlockchain] runtime output binary: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
	set(GradidoBlockchain_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
	message("[GradidoBlockchain] project binary folder: ${GradidoBlockchain_BINARY_DIR}")
ENDIF()

############################# add rust and iota.rs ####################
# https://webdevolutions.blob.core.windows.net/blog/pdf/insider-series-using-rust-code-in-a-cc-project-with-cmake.pdf
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake/")
if(ENABLE_IOTA_RUST_CLIENT)
	enable_language(Rust)
	include(CMakeCargo)
	add_subdirectory(iota_rust_clib)
endif()

############################ small libraries as subproject ###########
set(LOGURU_INSTALL ON)
set(BUILD_SHARED_LIBS_TEMP ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(dependencies/loguru)
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_TEMP})
add_subdirectory(dependencies/fkYAML)
# Set the policy default before adding the third-party directory
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
add_subdirectory(dependencies/miniz)

######################################################################

add_library(GradidoBlockchain ${GRADIDO_BLOCKCHAIN_LOCAL_SRCS})
add_library(GradidoBlockchain::GradidoBlockchain ALIAS GradidoBlockchain)

target_link_libraries(GradidoBlockchain loguru::loguru fkYAML::fkYAML miniz)
IF(WIN32) 
	if (TARGET ssl AND TARGET crypto)
		target_link_libraries(GradidoBlockchain ssl crypto)
	elseif(openssl_FOUND) 
		target_link_libraries(GradidoBlockchain openssl::openssl)
	endif()
	# needed for windows if linking to a rust lib
	if(ENABLE_IOTA_RUST_CLIENT)
		find_library(IOTA_RUST_C_LIB iota_rust_clib.dll.lib PATHS 
			"${PROJECT_BINARY_DIR}/iota_rust_clib/x86_64-pc-windows-msvc/${CMAKE_BUILD_TYPE}"
			"${PROJECT_BINARY_DIR}/iota_rust_clib/x86_64-pc-windows-msvc/debug"
			"${PROJECT_BINARY_DIR}/iota_rust_clib/x86_64-pc-windows-msvc/release"
		)
		IF(IOTA_RUST_C_LIB STREQUAL "IOTA_RUST_C_LIB-NOTFOUND" ) 
				message("[GradidoBlockchain] !!!!!!\nplease build target iota_rust_clib_target and call again `cmake`\n!!!!!!")
		ELSE()
			target_link_libraries(GradidoBlockchain ${IOTA_RUST_C_LIB})
		ENDIF()
	endif()
	target_link_libraries(GradidoBlockchain ws2_32 userenv)
ELSE()
	if(ENABLE_IOTA_RUST_CLIENT)
		target_link_libraries(GradidoBlockchain iota_rust_clib)	
	endif()
ENDIF()
target_link_libraries(GradidoBlockchain sodium)

set_target_properties(GradidoBlockchain PROPERTIES
                      VERSION ${GradidoBlockchain_VERSION}
                      SOVERSION ${GradidoBlockchain_VERSION_MAJOR})
target_include_directories(
    GradidoBlockchain PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_compile_features(GradidoBlockchain PUBLIC cxx_std_17)
target_compile_definitions(GradidoBlockchain PUBLIC -DRYML_SINGLE_HEADER)

include(GenerateExportHeader)
generate_export_header(GradidoBlockchain EXPORT_FILE_NAME include/gradido_blockchain/export.h)
target_compile_definitions(
    GradidoBlockchain PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:GRADIDOBLOCKCHAIN_STATIC_DEFINE>")
target_include_directories(
    GradidoBlockchain PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)
#option(GradidoBlockchain_INCLUDE_PACKAGING "Include packaging rules for GradidoBlockchain" "${is_top_level}")
#if (GradidoBlockchain_INCLUDE_PACKAGING)
 #   add_subdirectory(packaging)
#endif ()

if(ENABLE_TEST)
	# add test
	include(CTest)
	add_subdirectory(test)
endif()
