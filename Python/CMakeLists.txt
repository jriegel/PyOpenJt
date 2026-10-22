


include(GNUInstallDirs)

find_package(Python3 3.9 COMPONENTS Interpreter Development.SABIModule REQUIRED)

message(STATUS "******** PYTHON3_VERSION ${Python3_VERSION}")
message(STATUS "******** PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS}")
message(STATUS "******** PYTHON_SABI_LIBRARIES ${Python3_SABI_LIBRARIES}")


if(MSVC)
    #add_definitions(-D_3DLIFE_BUILD_)
else(MSVC)
    add_definitions(-Wno-write-strings)
endif(MSVC)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

get_target_property (TKJT_INCLUDE_DIRS TKJT INTERFACE_INCLUDE_DIRECTORIES)
get_target_property (ToolsLib_INCLUDE_DIRS ToolsLib INTERFACE_INCLUDE_DIRECTORIES)

add_definitions (-DTKJT_CERRTRACING)

if(WIN32)
  add_definitions (-DWNT -DWINVER=0x0500)
else()
  add_definitions (-fexceptions -fPIC)
endif()

# Suppression of 3rdparty warnings in MSVC with #pragma warnings (disable)
if(MSVC)
  add_definitions (/wd4718)
else()
  add_definitions (-Wno-unknown-pragmas)
endif()

   
include_directories(
    ${CMAKE_SOURCE_DIR}/TKJT/src/
    ${CMAKE_SOURCE_DIR}/ToolsLib/src/
) 




Python3_add_library(PyOpenJt MODULE USE_SABI 3.9 WITH_SOABI
	PyOpenJt.cpp
    JtFileObject.cpp
)

set_property(TARGET Python3::SABIModule PROPERTY MAP_IMPORTED_CONFIG_DEBUG RELEASE)

add_dependencies (PyOpenJt TKJT)
add_dependencies (PyOpenJt ToolsLib)


target_link_libraries(PyOpenJt PRIVATE 
    TKJT 
    ToolsLib
)

if(MSVC)
    set(PYTHON_DEBUG_LIBRARY_NAME "python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}_d.lib")
    target_link_options(PyOpenJt PRIVATE "$<$<CONFIG:Debug>:/NODEFAULTLIB:${PYTHON_DEBUG_LIBRARY_NAME}>")
endif()

set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR})
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo)
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/lib)
set_target_properties(PyOpenJt PROPERTIES OUTPUT_NAME PyOpenJt)
set_target_properties(PyOpenJt PROPERTIES DEBUG_POSTFIX "")
set_target_properties(PyOpenJt PROPERTIES PREFIX "")



if(SKBUILD)
    # Wheel layout: the module and TKJT sit side by side in site-packages.
    if(UNIX)
        set_target_properties(PyOpenJt TKJT PROPERTIES
            INSTALL_RPATH "$ORIGIN"
            BUILD_WITH_INSTALL_RPATH ON)
    endif()
    install(TARGETS PyOpenJt TKJT
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
    )
else()
    install(TARGETS PyOpenJt
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()