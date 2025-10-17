set(CMAKE_C_COMPILER zig cc)
set(CMAKE_CXX_COMPILER zig c++)
set(CMAKE_C_COMPILER_TARGET ${TARGET})
set(CMAKE_CXX_COMPILER_TARGET ${TARGET})

#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)




if( "${TARGET}" STREQUAL "aarch64-linux-gnu" AND NOT CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64")
  # CROSS COMPILING FOR ARM64 LINUX ON NON-ARM64 HOST
  message(STATUS "Cross compiling for arm64-linux-gnu")
  set(CMAKE_SYSTEM_NAME Linux)          # Target OS
  set(CMAKE_SYSTEM_PROCESSOR aarch64)  # Target CPU
  find_program(QEMU_AARCH64_EXECUTABLE qemu-aarch64)
  if(QEMU_AARCH64_EXECUTABLE)
    set(CMAKE_CROSSCOMPILING_EMULATOR ${QEMU_AARCH64_EXECUTABLE})
  else()
    message(WARNING "qemu-aarch64 not found, test don't run automatically")
  endif()
endif()

# set correct vcpkg triplet
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(VCPKG_TARGET_TRIPLET "arm64-linux")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-linux")
    endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(VCPKG_TARGET_TRIPLET "x64-windows")
    else()
        set(VCPKG_TARGET_TRIPLET "x86-windows")
    endif()
endif()