

cmake_minimum_required(VERSION 3.14.0)

project (PyOpenJt)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
# vmmlib still uses std::binary_function
add_compile_definitions (_HAS_DEPRECATED_RESULT_OF=1 _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS _HAS_AUTO_PTR_ETC=1)

# =============================================================================
# Configure CMake
# =============================================================================

set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  ${CMAKE_MODULE_PATH}
)

#include (DefineMacro)
include (DefineCxxFlags)



# =============================================================================
# Include projects or dependencies into solution
# =============================================================================


add_subdirectory (TKJT)
add_subdirectory (ToolsLib)
add_subdirectory (JtTools)
option (BUILD_JTASSISTANT "Build the JTAssistant viewer (not yet ported to the OpenCascade-free TKJT)" OFF)
if (BUILD_JTASSISTANT)
  add_subdirectory (JTAssistant)
endif()
add_subdirectory (Python)

