add_executable(fmt_tests
    ${CMAKE_CURRENT_LIST_DIR}/fmt_tests.cpp
)

target_link_libraries(fmt_tests
PRIVATE
    Catch2::Catch2WithMain
    fmt
)

add_test(NAME fmt_tests COMMAND fmt_tests)
