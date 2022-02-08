# Setup inspired from https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.18.2)
project(GradidoBlockchain VERSION 1.0.0)
#project(GradidoBlockchain C CXX)

if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET AND
    NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

if (DEFINED GradidoBlockchain_SHARED_LIBS)
    set(BUILD_SHARED_LIBS "${GradidoBlockchain_SHARED_LIBS}")
endif ()

FILE(GLOB TINF "dependencies/tinf/src/*.c" "dependencies/tinf/src/*.h")
FILE(GLOB LIB_SRC "include/gradido_blockchain/lib/*.h" "src/lib/*.cpp")
FILE(GLOB HTTP "include/gradido_blockchain/http/*.h" "src/http/*.cpp")
FILE(GLOB MODEL "include/gradido_blockchain/model/*.h" "src/model/*.cpp")
FILE(GLOB MODEL_PROTOBUF_WRAPPER "include/gradido_blockchain/model/protobufWrapper/*.h" "src/model/protobufWrapper/*.cpp")
FILE(GLOB CRYPTO "include/gradido_blockchain/crypto/*.h" "src/crypto/*.cpp")
FILE(GLOB MAIN "src/*.cpp" "src/*.c"  "include/gradido_blockchain/*.h")
FILE(GLOB PROTO_GRADIDO "build/proto/gradido/*.cc" "build/proto/gradido/*.h")

# used only for test project 
FILE(GLOB TEST "test/*.cpp" "src/cpp/test/*.h")
FILE(GLOB TEST_LIB "test/lib/*.cpp" "src/cpp/test/lib/*.h")

SET(LOCAL_SRCS 
	${TINF} ${MAIN} ${HTTP}
	${CRYPTO} ${MODEL} ${MODEL_PROTOBUF_WRAPPER}
	${SINGLETON_MANAGER} ${LIB_SRC} 
	${PROTO_GRADIDO} 
)

SET(TEST_SRCS
	${LOCAL_SRCS} ${TEST} ${TEST_LIB}
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
# CMake/FindSodium.cmake

add_library(GradidoBlockchain ${LOCAL_SRCS})
add_library(GradidoBlockchain::GradidoBlockchain ALIAS GradidoBlockchain)
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
