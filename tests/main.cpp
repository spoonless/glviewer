#include <iostream>
#include "gtest/gtest.h"
#include "gl.hpp"
#include "GlWindowContext.hpp"

int main(int argc, char **argv) {
    gl::GlWindowContext glwc;
    if(!glwc.init("unitttest", 1, 1) || !glwc.makeCurrent())
    {
        return 1;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
