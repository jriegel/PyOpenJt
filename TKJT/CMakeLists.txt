
# =============================================================================
# Look for required dependencies
# =============================================================================

find_package(Eigen3         CONFIG  REQUIRED)
find_package(TBB 			CONFIG 	REQUIRED)
find_package(ZLIB 					REQUIRED) 
find_package(liblzma        CONFIG  REQUIRED)


# =============================================================================
# Define production steps
# =============================================================================

set (TKJT_HEADER_DIRS)
set (TKJT_HEADERS)
set (TKJT_SOURCES)
set (TKJT_PACKAGES)

add_definitions (-DTKJT_CERRTRACING)

# =============================================================================
# Define production steps : search sources
# =============================================================================

set (CMAKE_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

file (GLOB SRC_DIRS RELATIVE ${CMAKE_SOURCE_PATH} ${CMAKE_SOURCE_PATH}/*)
foreach (SRC_DIR ${SRC_DIRS})
  if (IS_DIRECTORY ${CMAKE_SOURCE_PATH}/${SRC_DIR})
    list (APPEND TKJT_PACKAGES ${SRC_DIR})
  endif()
endforeach() 

foreach (TKJT_PACKAGE ${TKJT_PACKAGES})
  file (GLOB_RECURSE PACKAGE_HEADERS ${CMAKE_SOURCE_PATH}/${TKJT_PACKAGE}/*.hxx)
  file (GLOB_RECURSE PACKAGE_SOURCES ${CMAKE_SOURCE_PATH}/${TKJT_PACKAGE}/*.cxx)
  source_group ("Header Files\\${TKJT_PACKAGE}" FILES ${PACKAGE_HEADERS})
  source_group ("Source Files\\${TKJT_PACKAGE}" FILES ${PACKAGE_SOURCES})
  list (APPEND TKJT_SOURCES ${PACKAGE_SOURCES})
  list (APPEND TKJT_HEADERS ${PACKAGE_HEADERS})
  list (APPEND TKJT_HEADER_DIRS ${CMAKE_SOURCE_PATH}/${TKJT_PACKAGE})
endforeach()

include_directories (${TKJT_HEADER_DIRS})


add_library (TKJT SHARED ${TKJT_HEADERS} ${TKJT_SOURCES})


target_link_libraries(TKJT PUBLIC Eigen3::Eigen)
target_link_libraries(TKJT PRIVATE ZLIB::ZLIB)
target_link_libraries(TKJT PRIVATE TBB::tbb)
target_link_libraries(TKJT PRIVATE liblzma::liblzma)

if (BUILD_PROJECT STREQUAL "TKJT")
  set_target_properties (TKJT PROPERTIES PUBLIC_HEADER "${TKJT_HEADERS}")
endif()

set_target_properties (TKJT PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${TKJT_HEADER_DIRS}")

set_target_properties(TKJT PROPERTIES RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR})
set_target_properties(TKJT PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)
set_target_properties(TKJT PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo)
set_target_properties(TKJT PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug)
set_target_properties(TKJT PROPERTIES LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_BINARY_DIR}/lib)
set_target_properties(TKJT PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release/lib)
set_target_properties(TKJT PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/RelWithDebInfo/lib)
set_target_properties(TKJT PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/Debug/lib)

