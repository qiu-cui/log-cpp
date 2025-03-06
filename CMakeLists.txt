cmake_minimum_required(VERSION 3.15)

option(LOGCPP_BUILD_SHARED "build shared lib" OFF)
option(LOGCPP_SUPPORT_SQLITE "support sqlite" OFF)
option(LOGCPP_BUILD_TESTS "Build tests" OFF)

project(
    log-cpp
    VERSION 0.0.2
    DESCRIPTION "A simple logging library for C++"
)
#msvc 指定使用utf-8编码
if(MSVC)
	add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()


set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)

if(LOGCPP_BUILD_TESTS)
    add_subdirectory(test)
endif(LOGCPP_BUILD_TESTS)
