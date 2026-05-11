add_executable(example
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_link_libraries(example
PRIVATE
    fmt
    stl
)

set_property(TARGET example PROPERTY COMPILE_WARNING_AS_ERROR ON)
target_compile_options(example PRIVATE -Wall)
