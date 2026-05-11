include(${CMAKE_CURRENT_LIST_DIR}/include/fmt/CMakeLists.txt)

add_library(fmt INTERFACE
    ${FMT_HEADERS}
)

target_include_directories(fmt
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/fmt>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(fmt
    INTERFACE
        stl
)

# Export the target
include(CMakePackageConfigHelpers)

install(TARGETS fmt
    EXPORT fmtTargets
)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT fmtTargets
    FILE fmtTargets.cmake
    NAMESPACE fmt::
    DESTINATION lib/cmake/fmt
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/fmtConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/fmtConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/fmtConfigVersion.cmake"
    DESTINATION lib/cmake/fmt
)
