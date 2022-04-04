# Setup inspired from https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.18.2)
project(GradidoBlockchain VERSION 1.0.0)
#project(GradidoBlockchain C CXX)
IF(WIN32)
	set(CMAKE_CXX_FLAGS         "/EHsc /MP")
ENDIF()

if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
    NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED GradidoBlockchain_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${GradidoBlockchain_SHARED_LIBS}")
endif ()


############################## find protobuf  #########################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# prevent problems with two libs including googletest 
option(protobuf_BUILD_TESTS "Build tests" OFF)
add_subdirectory(dependencies/protobuf/cmake)

############################## parse protobuf files ###################################

FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/gradido_protocol GRADIDO_PROTO_MODEL_PATH)
FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build PROTOBINDING_PATH)
file(MAKE_DIRECTORY ${PROTOBINDING_PATH})
file(MAKE_DIRECTORY ${PROTOBINDING_PATH}/proto/gradido)
FILE(GLOB DATAMODEL_GRADIDO_PROTOS "${GRADIDO_PROTO_MODEL_PATH}/proto/gradido/*.proto")
include_directories(${PROTOBINDING_PATH})
IF(WIN32)
	set(PROTOC_BIN_PATH "${PROJECT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}")
	find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${PROTOC_BIN_PATH}" )
	IF(PROTOBUF_PROTOC_EXECUTABLE)
		MESSAGE("protoc: ${PROTOBUF_PROTOC_EXECUTABLE} in ${PROTOC_BIN_PATH}")
	ELSE()
		find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${CMAKE_BINARY_DIR}/bin" )
		MESSAGE("protoc: ${PROTOBUF_PROTOC_EXECUTABLE} in ${CMAKE_BINARY_DIR}/bin")
	ENDIF()
ELSE()
	find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${CMAKE_BINARY_DIR}/bin" )
	MESSAGE("protoc: ${PROTOBUF_PROTOC_EXECUTABLE} in ${CMAKE_BINARY_DIR}/bin")
ENDIF()

#MESSAGE("current src: ${CMAKE_CURRENT_SOURCE_DIR}, binary: ${CMAKE_BINARY_DIR}")
#MESSAGE("project src:${PROJECT_SOURCE_DIR}, project bin: ${PROJECT_BINARY_DIR}")
#MESSAGE("src: ${CMAKE_SOURCE_DIR}")

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
			MESSAGE("Generation of data model returned ${rv} for proto ${proto_native}")
		ELSE()
			MESSAGE("Parsed: src/proto/gradido/${proto_parsed}.proto")
		ENDIF()
   ENDIF()

ENDFOREACH(proto)

############################## bind source files ###################################

FILE(GLOB TINF "dependencies/tinf/src/*.c" "dependencies/tinf/src/*.h")
FILE(GLOB LIB_SRC "include/gradido_blockchain/lib/*.h" "src/lib/*.cpp")
FILE(GLOB HTTP "include/gradido_blockchain/http/*.h" "src/http/*.cpp")
FILE(GLOB MODEL "include/gradido_blockchain/model/*.h" "src/model/*.cpp")
FILE(GLOB MODEL_PROTOBUF_WRAPPER "include/gradido_blockchain/model/protobufWrapper/*.h" "src/model/protobufWrapper/*.cpp")
FILE(GLOB CRYPTO "include/gradido_blockchain/crypto/*.h" "src/crypto/*.cpp")
FILE(GLOB MAIN "src/*.cpp" "src/*.c"  "include/gradido_blockchain/*.h")
FILE(GLOB PROTO_GRADIDO "build/proto/gradido/*.cc" "build/proto/gradido/*.h")

SET(GRADIDO_BLOCKCHAIN_LOCAL_SRCS 
	${TINF} ${MAIN} ${HTTP}
	${CRYPTO} ${MODEL} ${MODEL_PROTOBUF_WRAPPER} ${LIB_SRC} 
	${PROTO_GRADIDO} 
)

if(MSVC)
	# src
	source_group("proto\\gradido" FILES ${PROTO_GRADIDO})
	source_group("tinf" FILES ${TINF})
	source_group("crypto" FILES ${CRYPTO})
	source_group("http" FILES ${HTTP})
	source_group("model\\protobufWrapper" FILES ${MODEL_PROTOBUF_WRAPPER})
	source_group("model" FILES ${MODEL})
	source_group("lib" FILES ${LIB_SRC})
endif()

include_directories(
	"dependencies" 
	"dependencies/tinf/src/" 
	"dependencies/protobuf/src"
	"dependencies/rapidjson/include"
	"include"
)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
IF(WIN32)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
ENDIF()

find_package(Sodium REQUIRED)
IF(UNIX)
	SET(ENABLE_MONGODB OFF CACHE BOOL "" FORCE) 
	SET(ENABLE_DATA_SQLITE OFF CACHE BOOL "" FORCE) 
	SET(ENABLE_REDIS OFF CACHE BOOL "" FORCE)
	SET(ENABLE_PAGECOMPILER_FILE2PAGE OFF CACHE BOOL "" FORCE)
	# TODO check if parent has already added poco
	add_subdirectory("dependencies/poco")

	set(POCO_LIBS PocoFoundation PocoUtil PocoNet PocoNetSSL PocoData)

ENDIF()

add_library(GradidoBlockchain ${GRADIDO_BLOCKCHAIN_LOCAL_SRCS})
add_library(GradidoBlockchain::GradidoBlockchain ALIAS GradidoBlockchain)

if(BUILD_SHARED_LIBS) 
	target_link_libraries(GradidoBlockchain libprotobuf)
	IF(WIN32) 
		target_link_libraries(GradidoBlockchain ${CONAN_LIBS})
	ELSE()
		target_link_libraries(GradidoBlockchain ${POCO_LIBS} sodium)
	ENDIF()
endif()
set_target_properties(GradidoBlockchain PROPERTIES
                      VERSION ${GradidoBlockchain_VERSION}
                      SOVERSION ${GradidoBlockchain_VERSION_MAJOR})
target_include_directories(
    GradidoBlockchain PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_compile_features(GradidoBlockchain PUBLIC cxx_std_17)

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
add_subdirectory("dependencies/protobuf/third_party/googletest")
add_subdirectory(test)
#add_test(NAME GradidoBlockchainTest
 #        COMMAND GradidoBlockchainTest)