=========
GL viewer
=========

A simple OpenGL 3.3+ viewer.

This implementation is based on :
* gl3w for OpenGL 3.3+ initialization (https://github.com/skaslev/gl3w)
* fglw for input/output management (http://www.glfw.org/)
* googletest for unit tests (https://code.google.com/p/googletest/)

All dependencies are downloaded, compiled and statically linked with cmake: easy build process and distribution!

Cross compilation from Linux to Windows is available thanks to mingw64 compiler with toochain file:

cmake -DCMAKE_TOOLCHAIN_FILE=${GLVIEWER_HOME}/cmake/toolchain-i686-w64-mingw32.cmake ${GLVIEWER_HOME}

GLSL viewer
-----------

Currently, you can visualize GLSL file from the really impressive GLSL Sandbox Gallery (http://glsl.heroku.com/).
Copy paste fragment shader source code on a local file and use glslviewer.exe:

Usage: glslviewer.exe [fragment shader file path]

