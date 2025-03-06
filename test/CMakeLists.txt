cmake_minimum_required(VERSION 3.15)

project(
    log_cpp_test
)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        log-cpp
)