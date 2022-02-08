include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (NOT DEFINED GradidoBlockchain_INSTALL_CMAKEDIR)
   set(GradidoBlockchain_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/GradidoBlockchain"
       CACHE STRING "Path to GradidoBlockchain CMake files")
endif ()

install(TARGETS GradidoBlockchain EXPORT GradidoBlockchain_Targets
    RUNTIME COMPONENT GradidoBlockchain_Runtime
    LIBRARY COMPONENT GradidoBlockchain_Runtime
    NAMELINK_COMPONENT GradidoBlockchain_Development
    ARCHIVE COMPONENT GradidoBlockchain_Development
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY "${GradidoBlockchain_SOURCE_DIR}/include/" "${GradidoBlockchain_BINARY_DIR}/include/"
    TYPE INCLUDE
    COMPONENT GradidoBlockchain_Development)

if (BUILD_SHARED_LIBS)
    set(type shared)
else ()
    set(type static)
endif ()
    
install(EXPORT GradidoBlockchain_Targets
    DESTINATION "${GradidoBlockchain_INSTALL_CMAKEDIR}"
    NAMESPACE GradidoBlockchain::
    FILE GradidoBlockchain-${type}-targets.cmake
    COMPONENT GradidoBlockchain_Development)

write_basic_package_version_file(
    GradidoBlockchainConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/GradidoBlockchainConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/GradidoBlockchainConfigVersion.cmake"
    DESTINATION "${GradidoBlockchain_INSTALL_CMAKEDIR}"
    COMPONENT GradidoBlockchain_Development)