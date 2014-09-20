=========
GL viewer
=========

A simple OpenGL 3.3+ viewer.

This implementation is based on :
* GLAD for OpenGL 3.3+ initialization (https://github.com/Dav1dde/glad)
* GLFW for input/output management (http://www.glfw.org/)
* GLM for mathematical computations (http://glm.g-truc.net)
* SOIL for texture files loading (https://github.com/kbranigan/Simple-OpenGL-Image-Library)
* googletest for unit tests (https://code.google.com/p/googletest/)

All dependencies are downloaded, compiled and statically linked with cmake: easy build process and distribution!

Cross compilation from Linux to Windows is available thanks to mingw64 compiler with toochain file:

cmake -DCMAKE_TOOLCHAIN_FILE=${GLVIEWER_HOME}/cmake/toolchain-i686-w64-mingw32.cmake ${GLVIEWER_HOME}

GLSL viewer
-----------

With glslviewer, you can execute GLSL shaders and optionally an OBJ file. 
Fragment shader must be a file with a .frag extension and vertex shader must be a file with a .vert extension.

For instance, you can visualize fragment shaders from the really impressive GLSL Sandbox Gallery (http://glslsandbox.com/).
Copy paste fragment shader source code on a local file and use glslviewer.exe:

Usage: glslviewer.exe mylocalfile.frag
