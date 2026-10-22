# This script configures project for JT Assistant application.
#

# =============================================================================
# Look for required dependencies
# =============================================================================

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find the QtWidgets library
find_package(Qt5			COMPONENTS Core Widgets REQUIRED)
find_package(TBB 			CONFIG 					REQUIRED)
find_package(ZLIB 									REQUIRED)
find_package(OpenGL 								REQUIRED)
find_package(Eigen3 								REQUIRED)


get_target_property (TKJT_INCLUDE_DIRS TKJT INTERFACE_INCLUDE_DIRECTORIES)
get_target_property (ToolsLib_INCLUDE_DIRS ToolsLib INTERFACE_INCLUDE_DIRECTORIES)

# =============================================================================
# Define production steps
# =============================================================================

include_directories ( 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/BVH
  ${CMAKE_CURRENT_SOURCE_DIR}/src/JTData
  ${CMAKE_CURRENT_SOURCE_DIR}/src/JTGui
  ${CMAKE_CURRENT_SOURCE_DIR}/src/JTVis
)

file (GLOB_RECURSE ProjectSources src/*.cxx)
file (GLOB_RECURSE ProjectHeaders src/*.hxx)
file (GLOB_RECURSE ProjectQRC *.qrc)
file (GLOB_RECURSE ProjectUI src/*.ui)


# Suppression of 3rdparty warnings in MSVC with #pragma warnings (disable)
if(MSVC)
  add_definitions (/wd4718)
else()
  add_definitions (-Wno-unknown-pragmas)
endif()


if (WIN32)
  enable_language (RC)
  set (EXE_ICON res/icons/desktop/icon.rc)
endif()

source_group ("Header Files" FILES
  ${ProjectHeaders}
)

source_group ("Resources" FILES
  ${ProjectUI}
  ${ProjectQRC}
  ${EXE_ICON}
)



add_executable (JTAssistant WIN32 ${ProjectSources} ${ProjectHeaders} ${ProjectUI} ${ProjectQRC} ${EXE_ICON})

add_dependencies (JTAssistant TKJT)
add_dependencies (JTAssistant ToolsLib)


target_link_libraries(JTAssistant PRIVATE ZLIB::ZLIB)
target_link_libraries(JTAssistant PRIVATE TBB::tbb)
target_link_libraries(JTAssistant PRIVATE Eigen3::Eigen)
target_link_libraries(JTAssistant PRIVATE OpenGL::GL)



target_link_libraries (JTAssistant PRIVATE TKJT)
target_link_libraries (JTAssistant PRIVATE ToolsLib)
target_link_libraries (JTAssistant PRIVATE Qt5::Widgets)

set_target_properties(JTAssistant PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR})
set_target_properties(JTAssistant PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(JTAssistant PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo)
set_target_properties(JTAssistant PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(JTAssistant PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
set_target_properties(JTAssistant PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set_target_properties(JTAssistant PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/lib)
set_target_properties(JTAssistant PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/lib)

