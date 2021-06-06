include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
    mcss
    PREFIX ${CMAKE_BINARY_DIR}/mcss
    GIT_REPOSITORY https://github.com/mosra/m.css.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

# Expose required variable (MCSS_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(mcss source_dir)
set(MCSS_SOURCE_DIR ${source_dir} CACHE INTERNAL "Path to include folder for mcss")

add_dependencies(${PROJECT_NAME} mcss)