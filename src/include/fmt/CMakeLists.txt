include(${CMAKE_CURRENT_LIST_DIR}/fmt/core/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/fmt/fmt_internal/CMakeLists.txt)

list(APPEND FMT_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/fmt/fmt.hpp
)

