cmake_minimum_required(VERSION 3.18.2)
cmake_policy(SET CMP0135 NEW)
project(GradidoBlockchainDataTest)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# collect source code files

FILE(GLOB TEST_MAIN "*.cpp" "*.h")


SET(TEST_SRC 
  ${TEST_MAIN}
)

if(MSVC)
	# src
	
endif()

IF(WIN32)
    # if static lib build
    message("build shared libs: ${BUILD_SHARED_LIBS}")
ENDIF()

enable_testing()


#find_package(Sodium REQUIRED)

add_executable(GradidoBlockchainDataTest ${TEST_SRC})
target_link_libraries(GradidoBlockchainDataTest GradidoBlockchain sodium loguru::loguru) #libprotobuf
target_link_libraries(GradidoBlockchainDataTest gtest)
if(openssl_FOUND) 
	target_link_libraries(GradidoBlockchainDataTest openssl::openssl)
endif()

include_directories(
	"../dependencies/date/include"
  "${googletest_SOURCE_DIR}/googletest/include"
)

IF(WIN32)
  add_custom_command(TARGET GradidoBlockchainDataTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:GradidoBlockchainDataTest> $<TARGET_FILE_DIR:GradidoBlockchainDataTest>
    COMMAND_EXPAND_LISTS
  )
ENDIF()

include(GoogleTest)
if(NOT CMAKE_CROSSCOMPILING OR (DEFINED CMAKE_CROSSCOMPILING_EMULATOR AND CMAKE_CROSSCOMPILING_EMULATOR))
  message(STATUS "Enable GradidoBlockchainDataTest tests")
  gtest_discover_tests(GradidoBlockchainDataTest)
endif()