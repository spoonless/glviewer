include(ExternalProject)
###############################################
# Download and compile SOIL
###############################################

if(CMAKE_CROSSCOMPILING AND CMAKE_TOOLCHAIN_FILE)
  get_filename_component(FULLPATH_CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE} REALPATH)
  message(${FULLPATH_CMAKE_TOOLCHAIN_FILE})
  set(GLFW_ADDITIONAL_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${FULLPATH_CMAKE_TOOLCHAIN_FILE})
endif()

ExternalProject_Add(
  project_soil
  URL https://github.com/spoonless/Simple-OpenGL-Image-Library/archive/originalfork.zip
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/soil"
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release ${GLFW_ADDITIONAL_CMAKE_ARGS}
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_soil SOURCE_DIR)
ExternalProject_Get_Property(project_soil BINARY_DIR)

add_library(soil STATIC IMPORTED)

set_property(TARGET soil PROPERTY IMPORTED_LOCATION "${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}SOIL${CMAKE_STATIC_LIBRARY_SUFFIX}")
# Handling multi configurations for MSVC
foreach( CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${CONFIG_TYPE} UPPER_CONFIG_TYPE)
  set_property(TARGET soil PROPERTY IMPORTED_LOCATION_${UPPER_CONFIG_TYPE} "${BINARY_DIR}/${CONFIG_TYPE}/${CMAKE_STATIC_LIBRARY_PREFIX}SOIL${CMAKE_STATIC_LIBRARY_SUFFIX}")
endforeach()
add_dependencies(soil project_soil)

set(SOIL_INCLUDE_DIR "${SOURCE_DIR}/src")
set(SOIL_LIBRARY soil)
