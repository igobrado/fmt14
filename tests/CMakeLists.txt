cmake_minimum_required(VERSION 3.14)
project(tests)

if (${ENABLE_TESTING})
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.4
    )
    FetchContent_MakeAvailable(Catch2)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fmt)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/stl)

endif()


if (${ENABLE_EXAMPLE_BUILD})
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/example)
endif()
