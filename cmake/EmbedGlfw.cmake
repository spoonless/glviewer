include(ExternalProject)
###############################################
# Download and compile GLFW
###############################################

if(CMAKE_CROSSCOMPILING AND CMAKE_TOOLCHAIN_FILE)
  get_filename_component(FULLPATH_CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE} REALPATH)
  message(${FULLPATH_CMAKE_TOOLCHAIN_FILE})
  set(GLFW_ADDITIONAL_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${FULLPATH_CMAKE_TOOLCHAIN_FILE})
endif()

ExternalProject_Add(
  project_glfw
  URL https://github.com/glfw/glfw/archive/3.0.4.zip
  URL_MD5 3dc81fc265df03715b1595e9cf80724e
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/glfw"
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DGLFW_BUILD_DOCS=0 -DGLFW_BUILD_EXAMPLES=0 -DGLFW_BUILD_TESTS=0 -DGLFW_INSTALL=0 ${GLFW_ADDITIONAL_CMAKE_ARGS}
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_glfw SOURCE_DIR)
ExternalProject_Get_Property(project_glfw BINARY_DIR)

add_library(glfw STATIC IMPORTED)

set_property(TARGET glfw PROPERTY IMPORTED_LOCATION "${BINARY_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}")
# Handling multi configurations for MSVC
foreach( CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${CONFIG_TYPE} UPPER_CONFIG_TYPE)
  set_property(TARGET glfw PROPERTY IMPORTED_LOCATION_${UPPER_CONFIG_TYPE} "${BINARY_DIR}/src/${CONFIG_TYPE}/${CMAKE_STATIC_LIBRARY_PREFIX}glfw3${CMAKE_STATIC_LIBRARY_SUFFIX}")
endforeach()

add_dependencies(glfw project_glfw)

set(GLFW_INCLUDE_DIR "${SOURCE_DIR}/include")
set(GLFW_LIBRARY glfw)
