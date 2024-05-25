# Setup inspired from https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.18.2)
cmake_policy(VERSION 3.18.2)
project(GradidoBlockchain VERSION 2.0.0)
#project(GradidoBlockchain C CXX)
IF(WIN32)
	set(CMAKE_CXX_FLAGS         "/MP /EHsc /std:c++17")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	set(BUILD_SHARED_LIBS ON)
ELSE()
	SET(CMAKE_CXX_FLAGS "-std=c++17" )# -fPIC" )
ENDIF()
#add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DNOMINMAX)

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

############################## find protobuf  #########################################

if(NOT TARGET protoc)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	message("[GradidoBlockchain] runtime output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

	# Set compile options for protobuf to ensure -fPIC is used
	set(protobuf_BUILD_TESTS OFF)  # Disable tests to avoid potential conflicts
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # This sets -fPIC for all targets

	add_subdirectory(dependencies/protobuf)

  # Force protobuf to use -fPIC
  set_target_properties(libprotobuf PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set_target_properties(protoc PROPERTIES POSITION_INDEPENDENT_CODE ON)	
endif()

############################## parse protobuf files ###################################

FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/gradido_protocol GRADIDO_PROTO_MODEL_PATH)
FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build PROTOBINDING_PATH)
file(MAKE_DIRECTORY ${PROTOBINDING_PATH})
file(MAKE_DIRECTORY ${PROTOBINDING_PATH}/proto/gradido)
FILE(GLOB DATAMODEL_GRADIDO_PROTOS "${GRADIDO_PROTO_MODEL_PATH}/proto/gradido/*.proto")
include_directories(${PROTOBINDING_PATH})
IF(WIN32)
	set(BUILD_TYPE ${CMAKE_BUILD_TYPE})
	IF(BUILD_TYPE STREQUAL "" OR NOT BUILD_TYPE)
		set(BUILD_TYPE "Debug")
	ENDIF()
	set(PROTOC_BIN_PATH "${PROJECT_BINARY_DIR}/bin/${BUILD_TYPE}")
	set(PROTOC_BIN_PATH2 "${CMAKE_BINARY_DIR}/bin/${BUILD_TYPE}")
	message("[GradidoBlockchain] protoc path: ${PROTOC_BIN_PATH2}")
	message("[GradidoBlockchain] cmake build type: ${CMAKE_BUILD_TYPE}")
	message("[GradidoBlockchain] build type: ${BUILD_TYPE}")
	find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${PROTOC_BIN_PATH}" "${PROTOC_BIN_PATH2}" "${PROJECT_BINARY_DIR}/bin/Debug" "${PROJECT_BINARY_DIR}/bin/Release")
	IF(NOT PROTOBUF_PROTOC_EXECUTABLE)
		find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${CMAKE_BINARY_DIR}/bin" )		
	ENDIF()
ELSE()
	find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${CMAKE_BINARY_DIR}/bin" "${PROJECT_SOURCE_DIR}/dependencies/protobuf/build" )
	MESSAGE("[GradidoBlockchain] protoc: ${PROTOBUF_PROTOC_EXECUTABLE} in ${PROJECT_SOURCE_DIR}/dependencies/gradido_blockchain/dependencies/protobuf/build")
ENDIF()

#MESSAGE("current src: ${CMAKE_CURRENT_SOURCE_DIR}, binary: ${CMAKE_BINARY_DIR}")
#MESSAGE("project src:${PROJECT_SOURCE_DIR}, project bin: ${PROJECT_BINARY_DIR}")
#MESSAGE("src: ${CMAKE_SOURCE_DIR}")
IF(PROTOBUF_PROTOC_EXECUTABLE STREQUAL "PROTOBUF_PROTOC_EXECUTABLE-NOTFOUND" ) 
  MESSAGE("[GradidoBlockchain] !!!!!!\nplease call `make protoc` to build the protoc compiler and call again `cmake`\n!!!!!!")
ELSE()
	FOREACH(proto ${DATAMODEL_GRADIDO_PROTOS})
	FILE(TO_NATIVE_PATH ${proto} proto_native)
	get_filename_component(proto_parsed ${proto} NAME_WLE)
	FILE(TO_NATIVE_PATH ${PROTOBINDING_PATH}/proto/gradido/${proto_parsed}.pb.h proto_parsed_native)


	IF(${proto_native} IS_NEWER_THAN ${proto_parsed_native})
		EXECUTE_PROCESS(
			COMMAND 
			${PROTOBUF_PROTOC_EXECUTABLE} 
			--proto_path=${GRADIDO_PROTO_MODEL_PATH} 
			--cpp_out=${PROTOBINDING_PATH} 
			${proto_native}
			RESULT_VARIABLE rv
		)
		# Optional, but that can show the user if something have gone wrong with the proto generation 
		IF(${rv})
			MESSAGE("[GradidoBlockchain] Generation of data model returned ${rv} for proto ${proto_native}")
		ELSE()
			MESSAGE("[GradidoBlockchain] Parsed: src/proto/gradido/${proto_parsed}.proto")
		ENDIF()
	ENDIF()

	ENDFOREACH(proto)
ENDIF()
#message("Proto: ${PROTO_FILES}")
############################## find dependencies ###################################
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(MPFR REQUIRED)
# using conan on windows
IF(WIN32)
  	find_package(libsodium REQUIRED)
	find_package(openssl REQUIRED)
	include_directories(
		${mpfr_INCLUDE_DIRS}
		${gmp_INCLUDE_DIRS}
		${libsodium_INCLUDE_DIR}
		${OPENSSL_INCLUDE_DIR}
	)
ELSE()
  find_package(Sodium)
ENDIF()

############################## bind source files ###################################
FILE(GLOB TINF "dependencies/tinf/src/*.c" "dependencies/tinf/src/*.h")
FIlE(GLOB THREADING "include/gradido_blockchain/threading/*.h" "src/threading/*.cpp")
FILE(GLOB LIB_SRC "include/gradido_blockchain/lib/*.h" "src/lib/*.cpp")
FILE(GLOB HTTP "include/gradido_blockchain/http/*.h" "src/http/*.cpp")
FILE(GLOB MODEL "include/gradido_blockchain/model/*.h" "src/model/*.cpp")
FILE(GLOB MODEL_PROTOBUF_WRAPPER "include/gradido_blockchain/model/protobufWrapper/*.h" "src/model/protobufWrapper/*.cpp")
FILE(GLOB MODEL_IOTA "include/gradido_blockchain/model/iota/*.h" "src/model/iota/*.cpp")
FILE(GLOB CRYPTO "include/gradido_blockchain/crypto/*.h" "src/crypto/*.cpp")
FILE(GLOB MAIN "src/*.cpp" "src/*.c"  "include/gradido_blockchain/*.h")
FILE(GLOB PROTO_GRADIDO "build/proto/gradido/*.cc" "build/proto/gradido/*.h")

SET(GRADIDO_BLOCKCHAIN_LOCAL_SRCS 
	${TINF} ${MAIN} ${HTTP} ${THREADING}
	${CRYPTO} ${MODEL} ${MODEL_PROTOBUF_WRAPPER} ${MODEL_IOTA} ${LIB_SRC} 
	${PROTO_GRADIDO}
)

if(MSVC)
	# src
	source_group("proto\\gradido" FILES ${PROTO_GRADIDO})
	source_group("threading" FILES ${THREADING})
	source_group("tinf" FILES ${TINF})
	source_group("crypto" FILES ${CRYPTO})
	source_group("http" FILES ${HTTP})
	source_group("model\\protobufWrapper" FILES ${MODEL_PROTOBUF_WRAPPER})
	source_group("model\\iota" FILES ${MODEL_IOTA})
	source_group("model" FILES ${MODEL})
	source_group("lib" FILES ${LIB_SRC})	
endif()

include_directories(
	"dependencies" 
	"dependencies/tinf/src/" 
	"dependencies/protobuf/src"
	"dependencies/loguru"
	"dependencies/rapidjson/include"
	#"dependencies/rapidyaml/src"
	#"dependencies/rapidyaml/ext/c4core/src"
	"dependencies/furi/code/"
	"dependencies/magic_enum/include"
	"dependencies/cpp-httplib"
	"dependencies/date/include"
	"include"
	"iota_rust_clib/include"	
	"dependencies/ed25519_bip32_c_interface/include"	
)


IF(BUILD_SHARED_LIBS)
ELSE()
  include_directories("dependencies/protobuf/third_party/abseil-cpp")
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
add_subdirectory(iota_rust_clib)
add_subdirectory(dependencies/ed25519_bip32_c_interface)

############################ small libraries as subproject ###########
set(LOGURU_INSTALL ON)
set(BUILD_SHARED_LIBS_TEMP ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(dependencies/loguru)
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_TEMP})


######################################################################

add_library(GradidoBlockchain ${GRADIDO_BLOCKCHAIN_LOCAL_SRCS})
add_library(GradidoBlockchain::GradidoBlockchain ALIAS GradidoBlockchain)

if(BUILD_SHARED_LIBS) 
	target_link_libraries(GradidoBlockchain libprotobuf loguru::loguru)
	IF(WIN32) 
	  target_link_libraries(GradidoBlockchain mpfr::mpfr libsodium::libsodium openssl::openssl)
		# needed for windows if linking to a rust lib
		find_library(IOTA_RUST_C_LIB iota_rust_clib.dll.lib PATHS 
			"${PROJECT_BINARY_DIR}/iota_rust_clib/x86_64-pc-windows-msvc/${CMAKE_BUILD_TYPE}"
			"${PROJECT_BINARY_DIR}/iota_rust_clib/x86_64-pc-windows-msvc/debug"
			"${PROJECT_BINARY_DIR}/iota_rust_clib/x86_64-pc-windows-msvc/release"
		)
		find_library(ED25519_BIP32_RUST_C_LIB ed25519_bip32_c_interface.dll.lib PATHS 
			"${PROJECT_BINARY_DIR}/dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/${CMAKE_BUILD_TYPE}"
			"${PROJECT_BINARY_DIR}/dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/debug"
			"${PROJECT_BINARY_DIR}/dependencies/ed25519_bip32_c_interface/x86_64-pc-windows-msvc/release"
		)
		IF(ED25519_BIP32_RUST_C_LIB STREQUAL "ED25519_BIP32_RUST_C_LIB-NOTFOUND" ) 
        	message("[GradidoBlockchain] !!!!!!\nplease build target ed25519_bip32_c_interface_target and call again `cmake`\n!!!!!!")
		ELSE()
			target_link_libraries(GradidoBlockchain ${ED25519_BIP32_RUST_C_LIB})
		ENDIF()

		IF(IOTA_RUST_C_LIB STREQUAL "IOTA_RUST_C_LIB-NOTFOUND" ) 
        	message("[GradidoBlockchain] !!!!!!\nplease build target iota_rust_clib_target and call again `cmake`\n!!!!!!")
		ELSE()
			target_link_libraries(GradidoBlockchain ${IOTA_RUST_C_LIB})
		ENDIF()
		target_link_libraries(GradidoBlockchain ws2_32 userenv)
	ELSE()
		target_link_libraries(GradidoBlockchain iota_rust_clib ed25519_bip32_c_interface ${MPFR_LIBRARIES})
	ENDIF()
else()
# static build
  target_link_libraries(GradidoBlockchain)
  target_link_libraries(GradidoBlockchain iota_rust_clib ed25519_bip32_c_interface)
endif()
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
option(GradidoBlockchain_INCLUDE_PACKAGING "Include packaging rules for GradidoBlockchain" "${is_top_level}")
if (GradidoBlockchain_INCLUDE_PACKAGING)
    add_subdirectory(packaging)
endif ()

# add test
include(CTest)
#add_subdirectory("dependencies/protobuf/third_party/googletest")
add_subdirectory(test)
#add_test(NAME GradidoBlockchainTest
 #        COMMAND GradidoBlockchainTest)