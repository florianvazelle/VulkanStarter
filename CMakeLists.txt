#
# VulkanStarter
#

cmake_minimum_required(VERSION 3.12)

include(cmake/tools/guards.cmake)

#
# Project configuration
#

project(
    VulkanStarter
    DESCRIPTION "A template for Vulkan C++ projects with GLFW, GLM and ImGUI using CMake, CI, Conan and doctest"
    HOMEPAGE_URL "https://github.com/florianvazelle/VulkanStarter"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)

# Vulkan is a required dependency.
include(FindVulkan)
if(NOT VULKAN_FOUND)
    message(FATAL_ERROR "Vulkan SDK not installed.")
endif()

include(cmake/tools/compile-shader.cmake)

# ---- Options ----

option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines." OFF)

#
# Define library target
#

file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")

# ---- Conan package ----

if(CONAN_EXPORTED)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    include("${CMAKE_SOURCE_DIR}/cmake/conan.cmake")
    conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP BUILD missing)
endif()

file(GLOB_RECURSE EXTERNALS "${CMAKE_CURRENT_BINARY_DIR}/external/*")


# ---- Create library ----

add_library(${PROJECT_NAME} OBJECT ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${EXTERNALS})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/external)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS} Vulkan::Vulkan)
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_VULKAN)

# target_set_warnings(${PROJECT_NAME} ENABLE ALL AS_ERROR ALL DISABLE Annoying)

# ---- Compile shader into SPIR-V ----

file(GLOB_RECURSE SHADERS "${CMAKE_SOURCE_DIR}/assets/shaders/*.vert" "${CMAKE_SOURCE_DIR}/assets/shaders/*.frag")

compile_shaders(TARGETS ${SHADERS})

#
# Entry points
#

option(BUILD_APPS "Enable building with entry points." ON)

if(BUILD_APPS)
    # Add an executable for the file app/main.cpp
    add_executable(${PROJECT_NAME}Standalone app/main.cpp)

    # Link the executable to library
    target_link_libraries(${PROJECT_NAME}Standalone PRIVATE ${PROJECT_NAME})

    option(INSTALL_APPS "Enable installation of the entry points." OFF)

    if (INSTALL_APPS)
        install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} DESTINATION .)
    endif()
endif()


#
# Tests
#

option(BUILD_TESTING "Enable building tests." OFF)

if(BUILD_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

#
# Documentation
#

option(BUILD_DOCS "Enable building with documentation." OFF)

if(BUILD_DOCS)
    find_package(Doxygen 1.8)

    if(DOXYGEN_FOUND)
        add_subdirectory(docs)
    endif()
endif()