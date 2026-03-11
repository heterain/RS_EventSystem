cmake_minimum_required(VERSION 3.10)

project(RSEventSystem VERSION 0.1.0 LANGUAGES C CXX)

# -----------------------------
# 0) 构建类型（兼容单/多配置生成器）
# -----------------------------
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

message(STATUS "Detected OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.20")
    set(RS_CXX_STD 23)
else()
    set(RS_CXX_STD 20)
    message(WARNING "CMake < 3.20 detected; fall back to C++20 to avoid C++23 configuration errors.")
endif()

option(RS_REQUIRE_CLANG "Require Clang/clang-cl to build" OFF)
if(RS_REQUIRE_CLANG AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    message(FATAL_ERROR "RS_REQUIRE_CLANG=ON, but compiler is '${CMAKE_CXX_COMPILER_ID}'. Please use Clang/clang-cl.")
endif()

set(_RS_BIN_DIR "${PROJECT_SOURCE_DIR}/bin")
set(_RS_LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

file(GLOB_RECURSE RS_source_code CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(${PROJECT_NAME} ${RS_source_code})
add_library(RS::EventSystem ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD ${RS_CXX_STD}
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${_RS_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${_RS_BIN_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${_RS_LIB_DIR}"
)

foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${cfg}" cfg_u)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${cfg_u} "${_RS_BIN_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_${cfg_u} "${_RS_BIN_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_${cfg_u} "${_RS_LIB_DIR}"
    )
endforeach()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        $<$<CONFIG:Debug>:_DEBUG>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _COMPILER_CLANG)
    if(MSVC)
        target_compile_definitions(${PROJECT_NAME} PUBLIC _COMPILER_MSVC_ABI)
    endif()
elseif(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _COMPILER_MSVC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(${PROJECT_NAME} PUBLIC _COMPILER_GCC)
endif()

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
target_compile_definitions(${PROJECT_NAME} PUBLIC RS_HAS_THREADS=1)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
endif()

include(CheckCXXSourceCompiles)

set(_RS_FS_TEST_CODE "
  #include <filesystem>
  int main() {
    std::filesystem::path p{\".\"};
    auto _ = std::filesystem::exists(p);
    (void)_;
    return 0;
  }
")

set(CMAKE_REQUIRED_LIBRARIES "")
check_cxx_source_compiles("${_RS_FS_TEST_CODE}" RS_FILESYSTEM_OK)

if(NOT RS_FILESYSTEM_OK)
    set(CMAKE_REQUIRED_LIBRARIES "stdc++fs")
    check_cxx_source_compiles("${_RS_FS_TEST_CODE}" RS_FILESYSTEM_OK_WITH_STDCXXFS)
    set(CMAKE_REQUIRED_LIBRARIES "")

    if(RS_FILESYSTEM_OK_WITH_STDCXXFS)
        target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
        target_compile_definitions(${PROJECT_NAME} PUBLIC RS_NEED_STDCXXFS=1)
        message(STATUS "std::filesystem needs stdc++fs: linked automatically.")
    else()
        message(FATAL_ERROR "std::filesystem is not usable on this toolchain/stdlib. Please update compiler/stdlib.")
    endif()
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC RS_HAS_FILESYSTEM=1)
endif()