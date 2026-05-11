add_executable(stl_tests
    ${CMAKE_CURRENT_LIST_DIR}/fixed_buffer_test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/traits_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/floating_point.cpp
)

target_link_libraries(stl_tests
PRIVATE
    Catch2::Catch2WithMain
    stl
)

add_test(NAME stl_tests COMMAND stl_tests)
