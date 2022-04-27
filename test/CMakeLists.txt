cmake_minimum_required(VERSION 3.18.2)
project(GradidoBlockchainTest)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

include_directories(
	"../dependencies/protobuf/third_party/googletest/googletest/include"
)

# collect source code files
FILE(GLOB TEST_MAIN "*.cpp" "*.h")
FILE(GLOB TEST_LIB "lib/*.cpp" "lib/*.h")

SET(TEST_SRC ${TEST_MAIN} ${TEST_LIB})

if(MSVC)
	# src
	source_group("lib" FILES ${TEST_LIB})
endif()

IF(WIN32)
    # if static lib build
    message("build shared libs: ${BUILD_SHARED_LIBS}")
    if(NOT BUILD_SHARED_LIBS)
	    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	    conan_basic_setup()
    ENDIF()
ENDIF()

enable_testing()

#include(FetchContent)
#FetchContent_Declare(
 # googletest
  #URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)

add_executable(GradidoBlockchainTest ${TEST_SRC})
target_link_libraries(GradidoBlockchainTest GradidoBlockchain libprotobuf gtest)

IF(NOT BUILD_SHARED_LIBS) 
    IF(WIN32) 
        target_link_libraries(GradidoBlockchainTest ${CONAN_LIBS})
    ENDIF()
ENDIF()

include(GoogleTest)
gtest_discover_tests(GradidoBlockchainTest)