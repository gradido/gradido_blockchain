# Setup inspired from https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.18.2)
cmake_policy(VERSION 3.18.2)
project(GradidoBlockchain VERSION 2.0.0)

option(ENABLE_IOTA_RUST_CLIENT "switch for include rust iota client into build, increase build time and size a lot" OFF)

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

# using conan on windows
IF(WIN32)
  find_package(libsodium REQUIRED)
	include_directories(${libsodium_INCLUDE_DIR})
	find_package(openssl)
	if(openssl_FOUND)
		include_directories(${OPENSSL_INCLUDE_DIR})
	endif()
ELSE()
  find_package(Sodium)
ENDIF()

############################## bind source files ###################################
FILE(GLOB CRYPTO "include/gradido_blockchain/crypto/*.h" "src/crypto/*.cpp")
FILE(GLOB DATA "include/gradido_blockchain/data/*.h" "src/data/*.cpp")
FILE(GLOB DATA_IOTA "include/gradido_blockchain/data/iota/*.h" "src/data/iota/*.cpp")
FILE(GLOB HTTP "include/gradido_blockchain/http/*.h" "src/http/*.cpp")
FILE(GLOB INTERACTION "include/gradido_blockchain/interaction/*.h" "src/interaction/*.cpp")

FILE(GLOB LIB_SRC "include/gradido_blockchain/lib/*.h" "src/lib/*.cpp")
FILE(GLOB MAIN "src/*.cpp" "src/*.c"  "include/gradido_blockchain/*.h")
FILE(GLOB MEMORY "include/gradido_blockchain/memory/*.h" "src/memory/*.cpp")
FILE(GLOB MODEL "include/gradido_blockchain/model/*.h" "src/model/*.cpp")
#FILE(GLOB MODEL_PROTOBUF_WRAPPER "include/gradido_blockchain/model/protobufWrapper/*.h" "src/model/protobufWrapper/*.cpp")
FILE(GLOB MODEL_PROTOPUF "include/gradido_blockchain/model/protopuf/*.h" "src/model/protopuf/*.cpp")
FILE(GLOB MODEL_IOTA "include/gradido_blockchain/model/iota/*.h" "src/model/iota/*.cpp")
FILE(GLOB PROTO_GRADIDO "build/proto/gradido/*.cc" "build/proto/gradido/*.h")
FILE(GLOB TINF "dependencies/tinf/src/*.c" "dependencies/tinf/src/*.h")
FIlE(GLOB THREADING "include/gradido_blockchain/threading/*.h" "src/threading/*.cpp")
FILE(GLOB BLOCKCHAIN "include/gradido_blockchain/blockchain/*.h" "src/blockchain/*.cpp")

FILE(GLOB INTERACTION_CALCULATE_ACCOUNT_BALANCE 
	"include/gradido_blockchain/interaction/calculateAccountBalance/*.h" 
	"src/interaction/calculateAccountBalance/*.cpp"
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
	${DATA} ${DATA_IOTA}
	#${HTTP}
	${INTERACTION}
	${LIB_SRC} 
	${MAIN}
	${MEMORY}
	#${MODEL} ${MODEL_PROTOBUF_WRAPPER} 
	${MODEL_IOTA} 
	#${MODEL_PROTOPUF} 
	#${PROTO_GRADIDO}
	${TINF} 
	${THREADING}
	${INTERACTION_CALCULATE_ACCOUNT_BALANCE}
	${INTERACTION_DESERIALIZE}
	${INTERACTION_SERIALIZE}
	${INTERACTION_TO_JSON}
	${INTERACTION_VALIDATE}
)

if(MSVC)
	# src
	source_group("blockchain" FILES ${BLOCKCHAIN})
	source_group("crypto" FILES ${CRYPTO})
	source_group("data\\iota" FILES ${DATA_IOTA})
	source_group("data" FILES ${DATA})
	source_group("http" FILES ${HTTP})
	source_group("interaction" FILES ${INTERACTION})
	source_group("lib" FILES ${LIB_SRC})	
	source_group("memory" FILES ${MEMORY})
	source_group("model\\iota" FILES ${MODEL_IOTA})
	source_group("model\\protocol\\3_3" FILES ${MODEL_PROTOCOL_3_3})
	source_group("model\\protobufWrapper" FILES ${MODEL_PROTOBUF_WRAPPER})
	#source_group("model\\protopuf" FILES ${MODEL_PROTOPUF})	
	source_group("model" FILES ${MODEL})
	#source_group("proto\\gradido" FILES ${PROTO_GRADIDO})
	source_group("tinf" FILES ${TINF})
	source_group("threading" FILES ${THREADING})	
	source_group("interaction\\calculateAccountBalance" FILES ${INTERACTION_CALCULATE_ACCOUNT_BALANCE})
	source_group("interaction\\deserialize" FILES ${INTERACTION_DESERIALIZE})
	source_group("interaction\\serialize" FILES ${INTERACTION_SERIALIZE})
	source_group("interaction\\toJson" FILES ${INTERACTION_TO_JSON})
	source_group("interaction\\validate" FILES ${INTERACTION_VALIDATE})
endif()

include_directories(
	"dependencies"
	"dependencies/tinf/src/" 
	"dependencies/protopuf/include"
	"dependencies/rapidjson/include"
	"dependencies/furi/code/"
	"dependencies/magic_enum/include"
	"dependencies/cpp-httplib"
	"dependencies/date/include"
	"include"
	"dependencies/ed25519_bip32_c_interface/include"	
)

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
enable_language(Rust)
include(CMakeCargo)
if(ENABLE_IOTA_RUST_CLIENT)
	add_subdirectory(iota_rust_clib)
endif()
add_subdirectory(dependencies/ed25519_bip32_c_interface)

############################ small libraries as subproject ###########
#option(LOGURU_INSTALL ON)
#set(BUILD_SHARED_LIBS_TEMP ${BUILD_SHARED_LIBS})
#set(BUILD_SHARED_LIBS OFF)
#message(${LOGURU_INSTALL})
add_subdirectory(dependencies/loguru)
# Exportiere loguru target in ein Set
#message(${LOGURU_INSTALL})
#set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_TEMP})


######################################################################

add_library(GradidoBlockchain ${GRADIDO_BLOCKCHAIN_LOCAL_SRCS})
add_library(GradidoBlockchain::GradidoBlockchain ALIAS GradidoBlockchain)

target_link_libraries(GradidoBlockchain PRIVATE loguru::loguru)
IF(WIN32) 
	if(openssl_FOUND) 
		target_link_libraries(GradidoBlockchain PRIVATE openssl::openssl)
	endif()
	target_link_libraries(GradidoBlockchain PRIVATE libsodium::libsodium)
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
			target_link_libraries(GradidoBlockchain PRIVATE ${IOTA_RUST_C_LIB})
		ENDIF()
	endif()
	find_library(ED25519_BIP32_RUST_C_LIB ed25519_bip32_c_interface.dll.lib PATHS 
		"${PROJECT_BINARY_DIR}/dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/${CMAKE_BUILD_TYPE}"
		"${PROJECT_BINARY_DIR}/dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/debug"
		"${PROJECT_BINARY_DIR}/dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/release"
	)
	IF(ED25519_BIP32_RUST_C_LIB STREQUAL "ED25519_BIP32_RUST_C_LIB-NOTFOUND" ) 
		message("[GradidoBlockchain] !!!!!!\nplease build target ed25519_bip32_c_interface_target and call again `cmake`\n!!!!!!")
	ELSE()
		target_link_libraries(GradidoBlockchain PRIVATE ${ED25519_BIP32_RUST_C_LIB})
	ENDIF()		
	target_link_libraries(GradidoBlockchain PRIVATE ws2_32 userenv)
ELSE()
	if(ENABLE_IOTA_RUST_CLIENT)
		target_link_libraries(GradidoBlockchain PRIVATE iota_rust_clib)	
	endif()
	target_link_libraries(GradidoBlockchain PRIVATE ed25519_bip32_c_interface)
ENDIF()

set_target_properties(GradidoBlockchain PROPERTIES
                      VERSION ${GradidoBlockchain_VERSION}
                      SOVERSION ${GradidoBlockchain_VERSION_MAJOR})
target_include_directories(
    GradidoBlockchain PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_compile_features(GradidoBlockchain PUBLIC cxx_std_17)


install(TARGETS loguru
    EXPORT LoguruTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS GradidoBlockchain
    EXPORT GradidoBlockchainTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# add loguru to GradidoBlockchain Export-Set
install(EXPORT LoguruTargets
    FILE LoguruTargets.cmake
    NAMESPACE Loguru::
    DESTINATION lib/cmake/Loguru
)

# add GradidoBlockchain to Export-Set and link to LoguruTargets
install(EXPORT GradidoBlockchainTargets
    FILE GradidoBlockchainTargets.cmake
    NAMESPACE GradidoBlockchain::
    DESTINATION lib/cmake/GradidoBlockchain
)

include(GenerateExportHeader)
generate_export_header(GradidoBlockchain EXPORT_FILE_NAME include/gradido_blockchain/export.h)
target_compile_definitions(
    GradidoBlockchain PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:GRADIDOBLOCKCHAIN_STATIC_DEFINE>")
target_include_directories(
    GradidoBlockchain PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" is_top_level)
option(GradidoBlockchain_INCLUDE_PACKAGING "Include packaging rules for GradidoBlockchain" "${is_top_level}")
if (GradidoBlockchain_INCLUDE_PACKAGING)
    add_subdirectory(packaging)
endif ()

# add test
include(CTest)

add_subdirectory(test)
