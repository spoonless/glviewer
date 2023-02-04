include(ExternalProject)
###############################################
# Download and compile gtest
# Note: pthread support is disabled for cross
# compilation
###############################################

if(CMAKE_CROSSCOMPILING AND CMAKE_TOOLCHAIN_FILE)
  get_filename_component(FULLPATH_CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE} REALPATH)
  message(${FULLPATH_CMAKE_TOOLCHAIN_FILE})
  set(GTEST_ADDITIONAL_CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${FULLPATH_CMAKE_TOOLCHAIN_FILE})
endif()

ExternalProject_Add(
  project_gtest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
  PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
  CMAKE_ARGS -Dgtest_disable_pthreads=ON -Dgtest_force_shared_crt=ON ${GTEST_ADDITIONAL_CMAKE_ARGS}
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_gtest SOURCE_DIR)
ExternalProject_Get_Property(project_gtest BINARY_DIR)

add_library(gtest STATIC IMPORTED)

set_property(TARGET gtest PROPERTY IMPORTED_LOCATION "${BINARY_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
# Handling multi configurations for MSVC
foreach( CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES} )
  string(TOUPPER ${CONFIG_TYPE} UPPER_CONFIG_TYPE)
  set_property(TARGET gtest PROPERTY IMPORTED_LOCATION_${UPPER_CONFIG_TYPE} "${BINARY_DIR}/${CONFIG_TYPE}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
endforeach()

add_dependencies(gtest project_gtest)

set(GTEST_INCLUDE_DIR "${SOURCE_DIR}/googletest/include")
set(GTEST_LIBRARY gtest)
