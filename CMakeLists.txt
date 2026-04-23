# Setup inspired from https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.18.2)
cmake_policy(VERSION 3.18.2)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake")
if(DEFINED ENV{GRADIDO_BLOCKCHAIN_VERSION})
    set(GRADIDO_BLOCKCHAIN_GIT_VER $ENV{GRADIDO_BLOCKCHAIN_VERSION})
else()
	execute_process(
		COMMAND git describe --tags --dirty --always
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE GRADIDO_BLOCKCHAIN_GIT_VER
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" GRADIDO_BLOCKCHAIN_CLEAN_VERSION "${GRADIDO_BLOCKCHAIN_GIT_VER}")
project(GradidoBlockchain VERSION ${GRADIDO_BLOCKCHAIN_CLEAN_VERSION})
option(ENABLE_IOTA_RUST_CLIENT "switch for include rust iota client into build, increase build time and size a lot" OFF)
option(ENABLE_HTTP "switch for include http based classes into build, increase build time and size" OFF)
option(ENABLE_HTTPS "switch for include openssl for https support" OFF)
option(ENABLE_TEST "switch for include test into build config" OFF)
option(USE_VCPKG "switch for use vcpkg for dependencies" OFF)
option(USE_INSTALLED_SODIUM "switch for use system installed libsodium" OFF)
option(USE_INSTALLED_SSL "Use the installed version of OpenSSL" OFF)

if(ENABLE_HTTPS)
  if (NOT USE_INSTALLED_SSL)
		set(USE_VCPKG ON CACHE BOOL "Use vcpkg for dependencies (auto-set because ENABLE_HTTPS is ON)" FORCE)
	endif()
	set(ENABLE_HTTP ON CACHE BOOL "Use http for dependencies (auto-set because ENABLE_HTTPS is ON)" FORCE)
endif()

IF(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
	set(CMAKE_CXX_FLAGS         "/MP /EHsc /std:c++20")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	set(BUILD_SHARED_LIBS ON)
ELSE()
	SET(CMAKE_CXX_FLAGS "-std=c++20" )# -fPIC" )
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
ENDIF()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_definitions(-DNOMINMAX)
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

# version
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gradido_blockchain/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/gradido_blockchain/version.h
    @ONLY
)

set(GRADIDO_BLOCKCHAIN_VERSION "@PROJECT_VERSION@")

############################## find dependencies ###################################
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (NOT BUILD_SHARED_LIBS)
	message(STATUS "add sodium static definition")
	add_definitions(-DSODIUM_STATIC)
endif()

	# Fetch Content Module
include(FetchContent)

set(SODIUM_DISABLE_TESTS ON)
if(USE_INSTALLED_SODIUM)
	find_package(Sodium REQUIRED)
	message(STATUS "Using installed Sodium library")
else()
	message(STATUS "Fetch Sodium library from GitHub")
	# Update the commit to point to whatever libsodium-cmake-commit you want to target.
	FetchContent_Declare(Sodium
			GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
			GIT_TAG e5b985ad0dd235d8c4307ea3a385b45e76c74c6a # HEAD, last updated at 2025-04-13
	)
	FetchContent_MakeAvailable(Sodium)
endif()

# Roaring Bitmaps
message(STATUS "Fetch Roaring Bitmap library from GitHub")
# detection seems not work always correct with zig as compiler, so we disable AVX512 support in roaring
set(ROARING_DISABLE_AVX512 ON)
FetchContent_Declare(
  roaring
  GIT_REPOSITORY https://github.com/RoaringBitmap/CRoaring.git
  GIT_TAG v4.6.1
  GIT_SHALLOW TRUE
)

set(ENABLE_ROARING_TESTS OFF CACHE INTERNAL "")

set(ROARING_BUILD_STATIC ON CACHE INTERNAL "")
FetchContent_MakeAvailable(roaring)

FetchContent_GetProperties(roaring)

if(USE_VCPKG)
	SET(VCPKG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/vcpkg")
	# Path to the vcpkg Toolchain in the submodule
	if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
		set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Vcpkg toolchain" FORCE)
	endif()

	if(NOT EXISTS "${VCPKG_PATH}/vcpkg" AND NOT EXISTS "${VCPKG_PATH}/vcpkg.exe")
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(VCPKG_BOOTSTRAP_CMD "${VCPKG_PATH}/bootstrap-vcpkg.bat")
    else()
        set(VCPKG_BOOTSTRAP_CMD "bash ${VCPKG_PATH}/bootstrap-vcpkg.sh")
    endif()
		message(STATUS "[GradidoBlockchain] Bootstrapping vcpkg...")
		execute_process(
			COMMAND ${VCPKG_BOOTSTRAP_CMD} -disableMetrics
			WORKING_DIRECTORY "${VCPKG_PATH}"
			RESULT_VARIABLE res
		)
		if(NOT res EQUAL 0)
			message(FATAL_ERROR "vcpkg bootstrap failed with exit code ${res}")
		endif()
	endif()

	if(ENABLE_HTTPS AND NOT USE_INSTALLED_SSL)
		# set correct vcpkg triplet
		if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
			set(VCPKG_TARGET_TRIPLET "arm64-")
		elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
			set(VCPKG_TARGET_TRIPLET "x64-")
		elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
			set(VCPKG_TARGET_TRIPLET "x86-")
		elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
			set(VCPKG_TARGET_TRIPLET "x64-")
		else()
			message(FATAL_ERROR "Unknown CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR}, cannot set VCPKG_TARGET_TRIPLET")
		endif()
		if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
				set(VCPKG_TARGET_TRIPLET "${VCPKG_TARGET_TRIPLET}linux")
		elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
				set(VCPKG_TARGET_TRIPLET "${VCPKG_TARGET_TRIPLET}windows")
		else()
				message(FATAL_ERROR "Unknown CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}, cannot set VCPKG_TARGET_TRIPLET")
		endif()

		message(STATUS "run vcpkg with VCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}")
		execute_process(
			COMMAND "${VCPKG_PATH}/vcpkg" install openssl:${VCPKG_TARGET_TRIPLET}
			WORKING_DIRECTORY "${VCPKG_PATH}"
			RESULT_VARIABLE vcpkg_install_result
		)
		if(NOT vcpkg_install_result EQUAL 0)
			message(FATAL_ERROR "vcpkg failed to install OpenSSL")
		endif()
	endif()
endif()

if(ENABLE_HTTPS)
	# CA certificates for used with cpp-httplib, make it also available for projects including this one
	set(CACERT_H "include/gradido_blockchain/cacert.h")
	set(CACERT_PEM "${CMAKE_BINARY_DIR}/cacert.pem" CACHE PATH "Path to CA certificates" FORCE)

	if(NOT EXISTS ${CACERT_PEM})
		message(STATUS "Downloading cacert.pem ...")
		file(DOWNLOAD
			https://curl.se/ca/cacert-2025-08-12.pem
			${CACERT_PEM}
			STATUS download_status
			EXPECTED_HASH SHA256=64dfd5b1026700e0a0a324964749da9adc69ae5e51e899bf16ff47d6fd0e9a5e
		)
		list(GET download_status 0 status_code)
		if(NOT status_code EQUAL 0)
			message(FATAL_ERROR "Failed to download cacert.pem: ${download_status}")
		endif()
	endif()

	add_custom_command(
			OUTPUT ${CACERT_H}
			COMMAND ${CMAKE_COMMAND}
					-DINPUT_FILE=${CACERT_PEM}
					-DOUTPUT_FILE=${CACERT_H}
					-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generate_cacert.cmake
			DEPENDS ${CACERT_PEM}
			COMMENT "Generating cacert.h from cacert.pem (using CMake script)"
			VERBATIM
	)

	add_custom_target(cacert_bundle DEPENDS ${CACERT_H})

	if(USE_VCPKG)
		set(OPENSSL_ROOT_DIR "${VCPKG_PATH}/installed/${VCPKG_TARGET_TRIPLET}" CACHE PATH "" FORCE)
	endif()
	if(NOT DBUILD_SHARED_LIBS)
		set(OPENSSL_USE_STATIC_LIBS TRUE)
	endif()
	find_package(OpenSSL REQUIRED)
	include_directories(${OPENSSL_INCLUDE_DIR})
	message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
	add_compile_definitions(USE_HTTPS)
endif()
# find_package(Sodium REQUIRED)

############################## bind source files ###################################
FILE(GLOB CRYPTO "include/gradido_blockchain/crypto/*.h" "src/crypto/*.cpp")
FILE(GLOB DATA "include/gradido_blockchain/data/*.h" "src/data/*.cpp")
FILE(GLOB DATA_ADAPTER "include/gradido_blockchain/data/adapter/*.h" "src/data/adapter/*.cpp")
FILE(GLOB DATA_COMPACT "include/gradido_blockchain/data/compact/*.h" "src/data/compact/*.cpp")
FILE(GLOB DATA_RICH "include/gradido_blockchain/data/rich/*.h" "src/data/rich/*.cpp")
FILE(GLOB DATA_IOTA "include/gradido_blockchain/data/iota/*.h" "src/data/iota/*.cpp")
FILE(GLOB DATA_HIERO "include/gradido_blockchain/data/hiero/*.h" "src/data/hiero/*.cpp")
FILE(GLOB HTTP "include/gradido_blockchain/http/*.h" "src/http/*.cpp")
FILE(GLOB INTERACTION "include/gradido_blockchain/interaction/*.h" "src/interaction/*.cpp")
FILE(GLOB LIB_SRC "include/gradido_blockchain/lib/*.h" "src/lib/*.cpp")
FILE(GLOB MAIN "src/*.cpp" "src/*.c"  "include/gradido_blockchain/*.h")
FILE(GLOB MEMORY "include/gradido_blockchain/memory/*.h" "src/memory/*.cpp")
FILE(GLOB PROTO_GRADIDO "build/proto/gradido/*.cc" "build/proto/gradido/*.h")
FILE(GLOB SERIALIZATION "include/gradido_blockchain/serialization/*.h" "src/serialization/*.cpp")
FILE(GLOB TINF "dependencies/tinf/src/*.c" "dependencies/tinf/src/*.h")
FIlE(GLOB THREADING "include/gradido_blockchain/threading/*.h" "src/threading/*.cpp")
FILE(GLOB BLOCKCHAIN "include/gradido_blockchain/blockchain/*.h" "src/blockchain/*.cpp")
FILE(GLOB BLOCKCHAIN_BATCH "include/gradido_blockchain/blockchain/batch/*.h" "src/blockchain/batch/*.cpp")


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
FILE(GLOB INTERACTION_VALIDATE
	"include/gradido_blockchain/interaction/validate/*.h"
	"src/interaction/validate/*.cpp"
)

SET(GRADIDO_BLOCKCHAIN_LOCAL_SRCS
	${BLOCKCHAIN} ${BLOCKCHAIN_BATCH}
	${CRYPTO}
	${DATA} ${DATA_ADAPTER} ${DATA_COMPACT} ${DATA_HIERO} ${DATA_IOTA} ${DATA_RICH}
	${INTERACTION}
	${LIB_SRC}
	${MAIN}
	${MEMORY}
	${MODEL_IOTA}
	${SERIALIZATION}
	${TINF}
	${THREADING}
	#${INTERACTION_ADVANCED_BLOCKCHAIN_FILTER}
	${INTERACTION_CALCULATE_ACCOUNT_BALANCE}
	${INTERACTION_CALCULATE_CREATION_SUM}
	${INTERACTION_CONFIRM_TRANSACTION}
	${INTERACTION_CREATE_TRANSACTION_BY_EVENT}
	${INTERACTION_DESERIALIZE}
	${INTERACTION_SERIALIZE}
	${INTERACTION_VALIDATE}
)

if(ENABLE_HTTP)
	SET(GRADIDO_BLOCKCHAIN_LOCAL_SRCS ${GRADIDO_BLOCKCHAIN_LOCAL_SRCS} ${HTTP})
endif()

if(MSVC)
	# src
	source_group("blockchain\\batch" FILES ${BLOCKCHAIN_BATCH})
	source_group("blockchain" FILES ${BLOCKCHAIN})	
	source_group("crypto" FILES ${CRYPTO})
	source_group("data\\adapter" FILES ${DATA_ADAPTER})
	source_group("data\\compact" FILES ${DATA_COMPACT})	
	source_group("data\\hiero" FILES ${DATA_HIERO})		
	source_group("data\\iota" FILES ${DATA_IOTA})
	source_group("data\\rich" FILES ${DATA_RICH})
	source_group("data" FILES ${DATA})
	if(ENABLE_HTTP)
		source_group("http" FILES ${HTTP})
	endif()
	source_group("interaction" FILES ${INTERACTION})
	source_group("lib" FILES ${LIB_SRC})
	source_group("memory" FILES ${MEMORY})
	source_group("serialization" FILES ${SERIALIZATION})
	source_group("tinf" FILES ${TINF})
	source_group("threading" FILES ${THREADING})
	source_group("interaction\\advancedBlockchainFilter" FILES ${INTERACTION_ADVANCED_BLOCKCHAIN_FILTER})
	source_group("interaction\\calculateAccountBalance" FILES ${INTERACTION_CALCULATE_ACCOUNT_BALANCE})
	source_group("interaction\\calculateCreationSum" FILES ${INTERACTION_CALCULATE_CREATION_SUM})
	source_group("interaction\\confirmTransaction" FILES ${INTERACTION_CONFIRM_TRANSACTION})
	source_group("interaction\\createTransactionByEvent" FILES ${INTERACTION_CREATE_TRANSACTION_BY_EVENT})
	source_group("interaction\\deserialize" FILES ${INTERACTION_DESERIALIZE})
	source_group("interaction\\serialize" FILES ${INTERACTION_SERIALIZE})
	source_group("interaction\\validate" FILES ${INTERACTION_VALIDATE})
endif()

include_directories(
	"dependencies"
	"dependencies/date/include"
	"dependencies/fkYAML/single_include"
	"dependencies/furi/code"
	"dependencies/gradido-blockchain-core/include"
	"dependencies/magic_enum/include"
	"dependencies/protopuf/include"
	"dependencies/rapidjson/include"
	"dependencies/r128"
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
# Set the policy default before adding the third-party directory
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
add_subdirectory(dependencies/gradido-blockchain-core)
add_subdirectory(dependencies/loguru)
add_subdirectory(dependencies/fkYAML)
add_subdirectory(dependencies/miniz)
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_TEMP})

######################################################################
# in zig rewritten parts
if(CMAKE_CROSSCOMPILING AND DEFINED TARGET)
    # Cross-Compilation: TARGET aus Toolchain-File nutzen
    set(ZIG_BUILD_TARGET "-Dtarget=${TARGET}")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    # Nativ auf aarch64
    set(ZIG_BUILD_TARGET "-Dtarget=aarch64-linux-gnu")
else()
    set(ZIG_BUILD_TARGET "")
endif()

add_custom_target(gradido_protobuf_zig_build
	COMMAND ${CMAKE_COMMAND} -E env zig build
			${ZIG_BUILD_TARGET}
			-Doptimize=ReleaseSmall
	WORKING_DIRECTORY
			${CMAKE_CURRENT_LIST_DIR}/dependencies/gradido_protobuf_zig
	VERBATIM
)

add_library(gradido_protobuf_zig STATIC IMPORTED)

set_target_properties(gradido_protobuf_zig PROPERTIES
    IMPORTED_LOCATION
        "${CMAKE_CURRENT_LIST_DIR}/dependencies/gradido_protobuf_zig/zig-out/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gradido_protobuf_zig${CMAKE_STATIC_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_LIST_DIR}/dependencies/gradido_protobuf_zig/include"
)
add_dependencies(gradido_protobuf_zig gradido_protobuf_zig_build)
######################################################################

add_library(GradidoBlockchain ${GRADIDO_BLOCKCHAIN_LOCAL_SRCS})
add_library(GradidoBlockchain::GradidoBlockchain ALIAS GradidoBlockchain)

if (ENABLE_HTTPS)
	# bundle cacert compressed into binary
	#set(CACERT_H "include/gradido_blockchain/cacert.h")
	#set(COMPRESS_CACERT "${CMAKE_BINARY_DIR}/compressCacert")
	#add_compile_definitions(CACERT_PEM_FILE_PATH="${CACERT_PEM}")
	#add_compile_definitions(CACERT_H_OUT_PATH="${CACERT_H}")

	#add_executable(compressCacert cmake/compressCacert.c)
	#target_include_directories(compressCacert PRIVATE dependencies/miniz)
	#target_link_libraries(compressCacert PRIVATE miniz sodium)

	# --- generate C-Header ---
	#add_custom_command(
	#		OUTPUT ${CACERT_H}
	#		COMMAND $<TARGET_FILE:compressCacert>
	#		DEPENDS compressCacert ${CACERT_PEM}
	#		COMMENT "Generating cacert.h from cacert.pem"
	#		VERBATIM
	#)
	#add_custom_command(
   # TARGET compressCacert POST_BUILD
    #COMMAND ${CMAKE_COMMAND} -E copy_if_different
     #       $<TARGET_FILE:sodium>
      #      $<TARGET_FILE_DIR:compressCacert>
	#)

	#add_custom_target(cacert_bundle DEPENDS ${CACERT_H})
	add_dependencies(${PROJECT_NAME} cacert_bundle)

	# Make the generated header file visible to the main project
	#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})
endif()

target_link_libraries(GradidoBlockchain loguru::loguru fkYAML::fkYAML miniz gradido_protobuf_zig roaring gradido_blockchain_core)
if(OpenSSL_FOUND)
	target_link_libraries(GradidoBlockchain OpenSSL::SSL OpenSSL::Crypto)
endif()
if(WIN32)
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
	add_subdirectory(tests)
endif()
