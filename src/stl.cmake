include(${CMAKE_CURRENT_LIST_DIR}/include/stl/CMakeLists.txt)

add_library(stl INTERFACE
    ${STL_HEADERS}
)

target_include_directories(stl
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/stl/>
        $<INSTALL_INTERFACE:include>
)

# Export the target
include(CMakePackageConfigHelpers)

install(TARGETS stl
    EXPORT stlTargets
)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT stlTargets
    FILE stlTargets.cmake
    NAMESPACE stl::
    DESTINATION lib/cmake/stl
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/stlConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/stlConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/stlConfigVersion.cmake"
    DESTINATION lib/cmake/stl
)
