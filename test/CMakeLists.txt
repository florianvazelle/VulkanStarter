cmake_minimum_required(VERSION 3.8)

# ---- Project ----
set(TEST_MAIN ${PROJECT_NAME}Tests)
set(TEST_RUNNER_PARAMS "")

# ---- Add source files ----
file(GLOB_RECURSE PROJECT_TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/src/*.cpp")

# ---- Create Entry Point ----
add_executable(${TEST_MAIN} ${PROJECT_TEST_SOURCES})

# Link the executable to library.
target_link_libraries(${TEST_MAIN} PRIVATE ${LIBRARY_NAME})

set_target_properties(${TEST_MAIN} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
    CXX_STANDARD ${PROJECT_CXX_STANDARD} 
    CXX_STANDARD_REQUIRED YES 
    CXX_EXTENSIONS NO
)

# add in ctest
add_test(
    # Use some per-module/project prefix so that it is easier to run only tests for this module
    NAME ${LIBRARY_NAME}.${TEST_MAIN}
    COMMAND ${TEST_MAIN} ${TEST_RUNNER_PARAMS})