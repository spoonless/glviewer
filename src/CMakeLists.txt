
include_directories(
    "${CMAKE_BINARY_DIR}/include"
)

configure_file("config.h.in" "${CMAKE_BINARY_DIR}/include/config.h")

#########################################################################
# sub modules
#########################################################################

add_module(glad)
add_module(sys)
add_module(ogl)

#########################################################################
# glviewer
#########################################################################

add_subdirectory(glviewer)
