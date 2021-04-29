set(target tests)
message(STATUS "${target}")

set(sources
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wuss_tests.cpp
)

file(CREATE_LINK ${CMAKE_BINARY_DIR}/wuss ${CMAKE_CURRENT_BINARY_DIR}/wuss SYMBOLIC)

add_executable(${target} ${sources})
target_link_libraries(${target}
    PRIVATE
    Catch2::Catch2
    Threads::Threads
)

target_compile_definitions(${target} PUBLIC
    WUSS_ROOT_DIR="${CMAKE_BINARY_DIR}"
)


add_test(NAME ${target} COMMAND $<TARGET_FILE:${target}>)
