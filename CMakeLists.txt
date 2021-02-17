cmake_minimum_required(VERSION 3.8)

# ---- Project ----

project(GreeterStandalone VERSION 1.2.3.4 LANGUAGES CXX)

set(LIBRARY_NAME Greeter)
set(PROJECT_CXX_STANDARD 11)

# ---- Options ----

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

# ---- Create library ----

add_library(${LIBRARY_NAME} OBJECT ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Lib needs its header files, and users of the library must also see these (PUBLIC).
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${LIBRARY_NAME} PUBLIC ${CONAN_LIBS})

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

add_compile_definitions(GREETER_VERSION="${CMAKE_PROJECT_VERSION}")

# ---- Auto-generated docs ----

if(ENABLE_DOC)
  add_subdirectory("${CMAKE_SOURCE_DIR}/docs")
endif()

# ---- Install ----

install(TARGETS ${PROJECT_NAME} DESTINATION bin)