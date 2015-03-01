include(ExternalProject)

###############################################
# Download GLM
###############################################
ExternalProject_Add(
  glm
  URL https://github.com/g-truc/glm/archive/0.9.6.3.zip
  URL_MD5 e7dc7970e1cb1a2a04874eddded0709e
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/glm"
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(glm SOURCE_DIR)

set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/dependency/include")
file(MAKE_DIRECTORY "${GLM_INCLUDE_DIR}/glm")

ExternalProject_Add_Step(glm "copy_include"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCE_DIR}/glm" "${GLM_INCLUDE_DIR}/glm" DEPENDEES install
)

# https://github.com/g-truc/glm/issues/283
# GLM does not compile with MSVC (32bits configuration) since 0.9.6
# Even if the issue is closed, it does not seem fixed in current version.
if(MSVC)
  add_definitions( -DGLM_FORCE_PURE )
endif()