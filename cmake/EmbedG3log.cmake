include(ExternalProject)
###############################################
# Download and compile g3log
###############################################

if(CMAKE_CROSSCOMPILING AND CMAKE_TOOLCHAIN_FILE)
  get_filename_component(FULLPATH_CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE} REALPATH)
  message(${FULLPATH_CMAKE_TOOLCHAIN_FILE})
  set(G3LOG_ADDITIONAL_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${FULLPATH_CMAKE_TOOLCHAIN_FILE})
endif()

ExternalProject_Add(
  project_g3log
  URL https://github.com/KjellKod/g3log/archive/refs/tags/2.1.zip
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/g3log"
  CMAKE_ARGS -DADD_FATAL_EXAMPLE=OFF -DG3_SHARED_LIB=OFF -DG3_SHARED_RUNTIME=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/g3log/install ${G3LOG_ADDITIONAL_CMAKE_ARGS}
  INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/g3log/install"
)

ExternalProject_Get_Property(project_g3log BINARY_DIR)
ExternalProject_Get_Property(project_g3log INSTALL_DIR)

add_library(g3log STATIC IMPORTED)

set_property(TARGET g3log PROPERTY IMPORTED_LOCATION "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}g3log${CMAKE_STATIC_LIBRARY_SUFFIX}")
# Handling multi configurations for MSVC
foreach( CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${CONFIG_TYPE} UPPER_CONFIG_TYPE)
  set_property(TARGET g3log PROPERTY IMPORTED_LOCATION_${UPPER_CONFIG_TYPE} "${INSTALL_DIR}/${CONFIG_TYPE}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}g3log${CMAKE_STATIC_LIBRARY_SUFFIX}")
endforeach()

add_dependencies(g3log project_g3log)

set(G3LOG_INCLUDE_DIR "${INSTALL_DIR}/include")
set(G3LOG_LIBRARY g3log)
if(MSVC OR MINGW)
    set(G3LOG_LIBRARY g3log dbghelp)
endif()
