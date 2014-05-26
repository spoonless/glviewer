# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)

add_definitions(-D__MSVCRT_VERSION__=0x0710)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER i586-mingw32msvc-gcc)
set(CMAKE_CXX_COMPILER i586-mingw32msvc-g++)
set(CMAKE_RC_COMPILER i586-mingw32msvc-windres)
set(CMAKE_RANLIB i586-mingw32msvc-ranlib)

# here is the target environment located
set(CMAKE_FIND_ROOT_PATH /usr/i586-mingw32msvc/)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


