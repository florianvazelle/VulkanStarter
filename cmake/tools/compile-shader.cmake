####################################################################################################
# This function compile any GLSL shader into SPIR-V shader and embed it in a C header file.
# Example:
#	compile_shaders(TARGETS "assets/shader/basic.frag" "assets/shader/basic.vert")
####################################################################################################

function(compile_shaders)

	include(CMakeParseArguments)
    cmake_parse_arguments(SHADERS "" "" "TARGETS" ${ARGN})

	# Note: if it remains unparsed arguments, here, they can be found in variable PARSED_ARGS_UNPARSED_ARGUMENTS
	if(NOT SHADERS)
		message(FATAL_ERROR "You must provide targets.")
	endif()

	# Find the glslangValidator executable
	if(WIN32)
		set(glslCompiler "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glslangValidator.exe")
	else()
		set(glslCompiler "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/glslangValidator")
	endif()

	# For each shader, we create a header file
	foreach(SHADER ${SHADERS})

		# Prepare a header name and a global variable for this shader
		get_filename_component(SHADER_NAME ${SHADER} NAME)
		string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
		string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)

		set(SHADER_HEADER "${PROJECT_SOURCE_DIR}/include/${HEADER_NAME}.h")

		add_custom_target(
			${HEADER_NAME}
			# Compile any GLSL shader into SPIR-V shader
			COMMAND ${glslCompiler} -V ${SHADER} -o ${SHADER}.spv
			# Make a C header file with the SPIR-V shader
			COMMAND ${CMAKE_COMMAND} -DPATH="${SHADER}.spv" -DHEADER="${SHADER_HEADER}" -DGLOBAL="${GLOBAL_SHADER_VAR}" -P "${CMAKE_SOURCE_DIR}/cmake/scripts/embed-data.cmake"
			# Rebuild the header file if the shader is updated
			DEPENDS ${SHADER}
			COMMENT "Building ${SHADER}.spv and embedding it into ${SHADER_HEADER}"
		)

		# Add the custom target like a dependencies of the project
		add_dependencies(${PROJECT_NAME} ${HEADER_NAME})

		message(STATUS "Generating build commands for ${SHADER}")
	endforeach()

endfunction()