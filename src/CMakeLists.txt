cmake_minimum_required(VERSION 3.15)

project(
    log-cpp
    VERSION 0.0.2
    DESCRIPTION "A simple logging library for C++"
)


if(NOT TARGET spdlog::spdlog_header_only)
find_package(spdlog CONFIG REQUIRED)
endif()


set(SQLiteSrc "")
set(SQLiteLink "")
if(LOGCPP_SUPPORT_SQLITE)
    if(NOT TARGET SQLiteCpp)
    find_package(SQLiteCpp CONFIG REQUIRED)
    endif()
    set(SQLiteSrc     
        ${CMAKE_CURRENT_SOURCE_DIR}/../inc/spdlog/sinks/sqlite_sink.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../inc/spdlog/details/sqlite_helper.h
        details/sqlite_helper.cpp
        sinks/sqlite_sink.cpp
    )
    set(SQLiteLink SQLiteCpp)

endif()
if(LOGCPP_BUILD_SHARED)
add_library(${PROJECT_NAME} 
    SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/w_log_cpp.h
    w_log_cpp.cpp
    details/singleton.h
    details/single_async_logger.h
    details/single_async_logger.cpp
    ${SQLiteSrc}
)
else()
add_library(${PROJECT_NAME} 
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/w_log_cpp.h
    w_log_cpp.cpp
    details/singleton.h
    details/single_async_logger.h
    details/single_async_logger.cpp
    ${SQLiteSrc}
)
endif() # LOGCPP_BUILD_SHARED

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})


set(CMAKE_INCLUDE_CURRENT_DIR ON)
target_compile_definitions(${PROJECT_NAME} PUBLIC LOGCPP_EXPORTS)

if(LOGCPP_BUILD_SHARED)
target_compile_definitions(${PROJECT_NAME} PRIVATE LOGCPP_SHARED_LIB)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../inc>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC
    spdlog::spdlog_header_only
    ${SQLiteLink}
)

install(
  TARGETS  ${PROJECT_NAME} DESTINATION lib
  EXPORT "${PROJECT_NAME}Targets"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(
    DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/../inc/"
    DESTINATION include
)

# Export targets
install(
  EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION lib/cmake/${PROJECT_NAME}
)

#--------------------------------------------------------------------
# Create generated files
#--------------------------------------------------------------------
include(CMakePackageConfigHelpers)
set(CONFIG_FILE_DIR ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CONFIG_FILE_DIR})
# Generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CONFIG_FILE_DIR}"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# Generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${CMAKE_PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

#--------------------------------------------------------------------
# Install CMake config files
#--------------------------------------------------------------------
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)