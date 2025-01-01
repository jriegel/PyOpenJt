# This script configures project for JtDump.
#


# =============================================================================
# Look for required dependencies
# =============================================================================

find_package(OpenCASCADE 	CONFIG 	REQUIRED)
find_package(CLI11			CONFIG	REQUIRED)
find_package(Eigen3 				REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

get_target_property (TKJT_INCLUDE_DIRS TKJT INTERFACE_INCLUDE_DIRECTORIES)
get_target_property (ToolsLib_INCLUDE_DIRS ToolsLib INTERFACE_INCLUDE_DIRECTORIES)

add_definitions (-DTKJT_CERRTRACING)

# =============================================================================
# Define production steps
# =============================================================================



if(WIN32)
  add_definitions (-DWNT -DWINVER=0x0500)
else()
  add_definitions (-fexceptions -fPIC -DOCC_CONVERT_SIGNALS)
endif()

# Suppression of 3rdparty warnings in MSVC with #pragma warnings (disable)
if(MSVC)
  add_definitions (/wd4718)
else()
  add_definitions (-Wno-unknown-pragmas)
endif()



# =============================================================================
# JtDump.exe
# =============================================================================


add_executable( JtDump  
	src/JtDump_Main.cpp
)

add_dependencies (JtDump TKJT)
add_dependencies (JtDump ToolsLib)

target_link_libraries (JtDump PRIVATE 
								TKJT 
								ToolsLib 
								CLI11::CLI11
								TKernel TKMath TKXSBase
								Eigen3::Eigen
						)
target_include_directories (JtDump PUBLIC ${OpenCASCADE_INCLUDE_DIR} )

set_target_properties(JtDump PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR})
set_target_properties(JtDump PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(JtDump PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo)
set_target_properties(JtDump PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(JtDump PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
set_target_properties(JtDump PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set_target_properties(JtDump PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/lib)
set_target_properties(JtDump PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/lib)

