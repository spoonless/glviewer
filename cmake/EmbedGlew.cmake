include(ExternalProject)

###############################################
# Download, compile and install locally luajit
###############################################
# Note: FFI is disabled for security reasons
ExternalProject_Add(
  project_glew
  URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.tgz/download
  URL_MD5 2f09e5e6cb1b9f3611bcac79bc9c2d5d
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/glew"
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} glew.lib
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_glew SOURCE_DIR)
set(GLEW_SOURCE_DIR "${SOURCE_DIR}/src")

add_library(glew STATIC IMPORTED)
set_property(TARGET glew PROPERTY IMPORTED_LOCATION "${SOURCE_DIR}/lib/libGLEW.a")
add_dependencies(glew project_glew)

set(GLEW_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")

set(GLEW_LIBRARY "glew")
