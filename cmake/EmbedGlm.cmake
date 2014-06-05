include(ExternalProject)

###############################################
# Download GLM
###############################################
ExternalProject_Add(
  glm
  URL https://github.com/g-truc/glm/archive/0.9.5.2.zip
  URL_MD5 34fff377f69fcea57191cdb7af279984
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
