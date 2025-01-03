


# Try to find the stable ABI for linking
#set (Python3_FIND_ABI "ANY" "ANY" "ANY" "ANY")

find_package(Python3 COMPONENTS Development REQUIRED)

 message(STATUS "******** PYTHON3_VERSION ${Python3_VERSION}")
 message(STATUS "******** PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS}")
 message(STATUS "******** PYTHON_LIBRARIES ${Python3_LIBRARIES}")

find_package(OpenCASCADE 	CONFIG 	REQUIRED)

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
  add_definitions (-fexceptions -fPIC -DOCC_CONVERT_SIGNALS)
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




link_directories(${PYTHON_INCLUDE_DIR}/../libs)


add_library(PyOpenJt SHARED 
	PyOpenJt.cpp
    JtFileObject.cpp
)
# use stable ABI of 3.7
#target_compile_definitions(PyOpenJt PUBLIC -DPy_LIMITED_API=0x030A0000)

add_dependencies (PyOpenJt TKJT)
add_dependencies (PyOpenJt ToolsLib)


target_link_libraries(PyOpenJt PRIVATE 
    Python3::Python 
    TKJT 
    ToolsLib
    TKernel TKMath TKXSBase
)
target_include_directories (PyOpenJt PUBLIC ${OpenCASCADE_INCLUDE_DIR} )

if(MSVC)
	set_target_properties(PyOpenJt PROPERTIES SUFFIX ".pyd")
endif(MSVC)

set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR})
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo)
set_target_properties(PyOpenJt PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/lib)
set_target_properties(PyOpenJt PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/lib)
set_target_properties(PyOpenJt PROPERTIES OUTPUT_NAME PyOpenJt)
set_target_properties(PyOpenJt PROPERTIES DEBUG_OUTPUT_NAME PyOpenJt_d)
set_target_properties(PyOpenJt PROPERTIES PREFIX "")



if(WIN32)
    INSTALL(TARGETS PyOpenJt
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
else(WIN32)
    INSTALL(TARGETS PyOpenJt
        RUNTIME DESTINATION ~/bin
        LIBRARY DESTINATION ~/lib
        ARCHIVE DESTINATION ~/lib
   )
endif(WIN32)