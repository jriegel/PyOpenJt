# This script configures project for ToolsLib.
#


# =============================================================================
# Look for required dependencies
# =============================================================================

find_package(OpenCASCADE 	CONFIG 	REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")

find_package(draco CONFIG REQUIRED)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

get_target_property (TKJT_INCLUDE_DIRS TKJT INTERFACE_INCLUDE_DIRECTORIES)

# =============================================================================
# Define production steps
# =============================================================================

 

#file (GLOB_RECURSE ProjectSources src/*.cpp)
set (ProjectSources
	src/JtData2Json.cpp 
	src/JtLayer.cpp 
	src/JtUsdMesh.cpp 
	src/JtGltfMesh.cpp
	src/JtUtils.cpp
)

#file (GLOB_RECURSE ProjectHeaders src/*.h)
set (ProjectHeaders 
	src/JtData2Json.h 
	src/JtLayer.h 
	src/JtUsdMesh.h 
	src/JtGltfMesh.h 
	src/JtUtils.h
)

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

source_group ("Header Files" FILES
  ${ProjectHeaders}
)


add_library( ToolsLib  STATIC 
	${ProjectSources}
	${ProjectHeaders} 
)

add_dependencies (ToolsLib TKJT)

target_link_libraries (ToolsLib PRIVATE TKJT  draco::draco)

target_include_directories (ToolsLib 
								PUBLIC 
									${OpenCASCADE_INCLUDE_DIR} 
									${ProjectHeaders}
									${TINYGLTF_INCLUDE_DIRS}
							)

set_target_properties (ToolsLib PROPERTIES PUBLIC_HEADER "${ProjectHeaders}")
set_target_properties (ToolsLib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/src")

set_target_properties(ToolsLib PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR})
set_target_properties(ToolsLib PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(ToolsLib PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo)
set_target_properties(ToolsLib PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(ToolsLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
set_target_properties(ToolsLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set_target_properties(ToolsLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/lib)
set_target_properties(ToolsLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/lib)

