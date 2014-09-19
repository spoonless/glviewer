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
  URL https://github.com/spoonless/Simple-OpenGL-Image-Library/archive/master.zip
  URL_MD5 4d007a67d4acaef936397525ee0769a3
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/soil"
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release ${GLFW_ADDITIONAL_CMAKE_ARGS}
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_soil SOURCE_DIR)
ExternalProject_Get_Property(project_soil BINARY_DIR)

add_library(soil STATIC IMPORTED)
set_property(TARGET soil PROPERTY IMPORTED_LOCATION "${BINARY_DIR}/libSOIL.a")
add_dependencies(soil project_soil)

set(SOIL_INCLUDE_DIR "${SOURCE_DIR}/src")
set(SOIL_LIBRARY soil)
