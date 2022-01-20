cmake_minimum_required(VERSION 3.18.2)
project(GradidoBlockchain C CXX)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin" )

SET(CMAKE_CXX_FLAGS "-std=c++17" )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(WIN32)
	set(CMAKE_CXX_FLAGS         "/MP /EHsc")
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
ENDIF()

set(INSTALL_BINDIR "bin")
set(INSTALL_PLUGINDIR "bin")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(
	"dependencies" 
	"dependencies/tinf/src/" 
	"dependencies/protobuf/src"
	"dependencies/rapidjson/include"
	"src/cpp"
)

IF(UNIX)
	include_directories(
		"dependencies/poco/Crypto/include" 
		"dependencies/poco/Data/include" 
		"dependencies/poco/Util/include" 
		"dependencies/poco/Foundation/include"
		"dependencies/poco/JSON/include"
		"dependencies/poco/Net/include"
		"dependencies/poco/NetSSL_OpenSSL/include"
	)
ENDIF()

############################## find protobuf  #########################################
add_subdirectory(dependencies/protobuf/cmake)

############################## parse protobuf files ###################################

FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/proto/gradido_protocol GRADIDO_PROTO_MODEL_PATH)
FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/proto PROTOBINDING_PATH)
file(MAKE_DIRECTORY ${PROTOBINDING_PATH})
file(MAKE_DIRECTORY ${PROTOBINDING_PATH}/gradido)
FILE(GLOB DATAMODEL_GRADIDO_PROTOS "${GRADIDO_PROTO_MODEL_PATH}/gradido/*.proto")
include_directories(${PROTOBINDING_PATH})
IF(WIN32)
	set(PROTOC_BIN_PATH "${CMAKE_BINARY_DIR}/bin")
	find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${PROTOC_BIN_PATH}/Debug" )
	find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${PROTOC_BIN_PATH}/Release" )
ELSE()
	find_program(PROTOBUF_PROTOC_EXECUTABLE protoc PATHS "${CMAKE_BINARY_DIR}/bin" )
ENDIF()
MESSAGE("protoc: ${PROTOBUF_PROTOC_EXECUTABLE} in ${PROTOC_BIN_PATH}/${CMAKE_BUILD_TYPE}")
FOREACH(proto ${DATAMODEL_GRADIDO_PROTOS})
   FILE(TO_NATIVE_PATH ${proto} proto_native)
   get_filename_component(proto_parsed ${proto} NAME_WLE)
   FILE(TO_NATIVE_PATH ${PROTOBINDING_PATH}/gradido/${proto_parsed}.pb.h proto_parsed_native)
   
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
FILE(GLOB SINGLETON_MANAGER "src/cpp/SingletonManager/*.h" "src/cpp/SingletonManager/*.cpp")
FILE(GLOB LIB_SRC "src/cpp/lib/*.h" "src/cpp/lib/*.cpp")
FILE(GLOB HTTP "src/cpp/http/*.h" "src/cpp/http/*.cpp")
FILE(GLOB MODEL "src/cpp/model/*.h" "src/cpp/model/*.cpp")
FILE(GLOB MODEL_GRADIDO "src/cpp/model/gradido/*.h" "src/cpp/model/gradido/*.cpp")
FILE(GLOB MODEL_PROTOBUF_WRAPPER "src/cpp/model/protobufWrapper/*.h" "src/cpp/model/protobufWrapper/*.cpp")
FILE(GLOB CRYPTO "src/cpp/Crypto/*.h" "src/cpp/Crypto/*.cpp")
FILE(GLOB MAIN "src/cpp/*.cpp" "src/cpp/*.c"  "src/cpp/*.h")
FILE(GLOB PROTO_GRADIDO "build/proto/gradido/*.cc" "build/proto/gradido/*.h")

SET(LOCAL_SRCS 
	${TINF} ${MAIN} ${HTTP}
	${CRYPTO} ${MODEL} ${MODEL_GRADIDO} ${MODEL_PROTOBUF_WRAPPER}
	${SINGLETON_MANAGER} ${LIB_SRC} 
	${PROTO_GRADIDO} 
)

aux_source_directory("src/cpp" LOCAL_SRCS)

if(MSVC)
	# src
	source_group("proto\\gradido" FILES ${PROTO_GRADIDO})
	source_group("tinf" FILES ${TINF})
	source_group("Crypto" FILES ${CRYPTO})
	source_group("http" FILES ${HTTP})
	source_group("model\\gradido" FILES ${MODEL_GRADIDO})
	source_group("model\\protobufWrapper" FILES ${MODEL_PROTOBUF_WRAPPER})
	source_group("model" FILES ${MODEL})
	source_group("SingletonManager" FILES ${SINGLETON_MANAGER})
	source_group("lib" FILES ${LIB_SRC})
endif()

if(WIN32)
	add_library(GradidoBlockchain STATIC ${LOCAL_SRCS})
else()
	add_library(GradidoBlockchain DYNAMIC ${LOCAL_SRCS})
endif()

############################## config and add poco ###################################
#SET(SOME_EXPAT_OPTION OFF CACHE BOOL "Use some expat option")
IF(UNIX)
	SET(ENABLE_MONGODB OFF CACHE BOOL "" FORCE) 
	SET(ENABLE_DATA_SQLITE OFF CACHE BOOL "" FORCE) 
	SET(ENABLE_REDIS OFF CACHE BOOL "" FORCE)
	SET(ENABLE_PAGECOMPILER_FILE2PAGE OFF CACHE BOOL "" FORCE)
	add_subdirectory("dependencies/poco")

	set(POCO_LIBS PocoFoundation PocoUtil PocoNet PocoNetSSL PocoData)
ENDIF()

############################## build blockchain lib ###################################

if(WIN32)
	target_link_libraries(GradidoBlockchain libprotobuf ${CONAN_LIBS})
else() # unix
	target_link_libraries(GradidoBlockchain ${POCO_LIBS} sodium libprotobuf)
endif()

