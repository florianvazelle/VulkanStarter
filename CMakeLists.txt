cmake_minimum_required(VERSION 3.8)

# ---- Project ----

project(VulkanStarterStandalone VERSION 1.2.3.4 LANGUAGES CXX)

set(LIBRARY_NAME VulkanStarter)
set(PROJECT_CXX_STANDARD 17) # for <optional>

include(FindVulkan)
if(NOT VULKAN_FOUND)
  message(FATAL_ERROR "Vulkan SDK not installed.")
endif()

# ---- Options ----

option(ENABLE_TESTING "Whether to generate unit test program and test" ON)
option(ENABLE_DOC "Whether to generate documentation" OFF)

# ---- Add source files ----

file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE PROJECT_ALL_FILES "include/*.hpp" "src/*.cpp" "app/*.cpp")

# ---- Tool ----

file(GLOB_RECURSE PROJECT_MODULES "${CMAKE_SOURCE_DIR}/cmake/*.cmake")
foreach(ARG ${PROJECT_MODULES})
  include(${ARG})
endforeach()

# ---- Conan package ----

if(CONAN_EXPORTED)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    include("${CMAKE_SOURCE_DIR}/cmake/manual/conan.cmake")
    conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP BUILD missing)
endif()

file(GLOB_RECURSE EXTERNALS "${CMAKE_CURRENT_BINARY_DIR}/external/*")

# ---- Shaders ----
file(GLOB_RECURSE SHADERS "${CMAKE_SOURCE_DIR}/assets/shader/*.vert" "${CMAKE_SOURCE_DIR}/assets/shader/*.frag")

# ---- Create library ----

add_library(${LIBRARY_NAME} OBJECT ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${EXTERNALS})

# Lib needs its header files, and users of the library must also see these (PUBLIC).
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/external)

target_link_libraries(${LIBRARY_NAME} PUBLIC ${CONAN_LIBS} Vulkan::Vulkan)
target_compile_definitions(${LIBRARY_NAME} PUBLIC IMGUI_IMPL_VULKAN)

# ---- Create Entry Point ----

# Add an executable for the file app/main.cc.
# If you add more executables, copy these lines accordingly.
add_executable(${PROJECT_NAME} app/main.cpp)

# Link the executable to library.
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARY_NAME})

# Set the properties you require, e.g. what C++ standard to use. Here applied to library and main.
set_target_properties(
      ${PROJECT_NAME} ${LIBRARY_NAME}
      PROPERTIES 
        CXX_STANDARD ${PROJECT_CXX_STANDARD} 
        CXX_STANDARD_REQUIRED YES 
        CXX_EXTENSIONS NO
)

add_compile_definitions(ENGINE_NAME="No Engine")
add_compile_definitions(APP_NAME="${PROJECT_NAME}")

# Set up tests (see test/CMakeLists.txt).
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory("${CMAKE_SOURCE_DIR}/test")
endif()

# ---- Auto-generated docs ----

if(ENABLE_DOC)
  add_subdirectory("${CMAKE_SOURCE_DIR}/docs")
endif()

# ---- Compile shader into SPIR-V ----

if (WIN32)
  set(glslCompiler "glslangValidator.exe")
else()
  set(glslCompiler "glslangValidator")
endif()

foreach(SHADER ${SHADERS})
  # Prepare a header and a global variable for this shader
  get_filename_component(SHADER_NAME ${SHADER} NAME)
  string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
  string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)

  set(SHADER_HEADER "${PROJECT_SOURCE_DIR}/include/${HEADER_NAME}")

  add_custom_target(
    ${HEADER_NAME}
    COMMAND ${PROJECT_BINARY_DIR}/bin/${glslCompiler} -V ${SHADER} -o ${SHADER}.spv
    COMMAND ${CMAKE_COMMAND} -DPATH="${SHADER}.spv" -DHEADER="${SHADER_HEADER}" -DGLOBAL="${GLOBAL_SHADER_VAR}" -P "${CMAKE_SOURCE_DIR}/cmake/scripts/EmbedData.cmake"
    DEPENDS ${SHADER}
    COMMENT "Building ${SHADER}.spv and embedding it into ${SHADER_HEADER}.h"
  )

  add_dependencies(${LIBRARY_NAME} ${HEADER_NAME})

  message(STATUS "Generating build commands for ${SHADER}")
endforeach()

# ---- Install ----

install(TARGETS ${PROJECT_NAME} DESTINATION bin)