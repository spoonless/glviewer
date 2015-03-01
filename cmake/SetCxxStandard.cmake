# Code snippet inspired from http://www.guyrutenberg.com/2014/01/05/enabling-c11-c0x-in-cmake/

include(CheckCXXCompilerFlag)

macro( set_cxx_standard VERSION )
  if(CMAKE_COMPILER_IS_GNUCXX)
    check_cxx_compiler_flag("-std=c++${VERSION}" "COMPILER_SUPPORTS_CXX_${VERSION}")
    if(${COMPILER_SUPPORTS_CXX_${VERSION}})
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${VERSION}")
    else(VERSION NOT STREQUAL "98")
      message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++${VERSION} support.")
    endif()
  else()
    message(STATUS "No specific operation done to enable C++ ${VERSION}")
  endif()
endmacro()

