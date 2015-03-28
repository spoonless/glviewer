#include <gtest/gtest.h>
#include "glm/gtx/transform.hpp"
#include "Camera.hpp"

using namespace glv;

TEST(PerspectiveCamera, canGetViewportDimension800x600)
{
    PerspectiveCamera pc;
    unsigned int viewportWidth = 800;
    unsigned int viewportHeight = 600;

    pc.aspectRatio(viewportWidth, viewportHeight);

    ASSERT_EQ(viewportHeight, pc.viewportHeight(viewportWidth));
    ASSERT_EQ(viewportWidth, pc.viewportWidth(viewportHeight));
}

TEST(PerspectiveCamera, canGetViewportDimension1920x1080)
{
    PerspectiveCamera pc;
    unsigned int viewportWidth = 1920;
    unsigned int viewportHeight = 1080;

    pc.aspectRatio({viewportWidth, viewportHeight});

    ASSERT_EQ(viewportHeight, pc.viewportHeight(viewportWidth));
    ASSERT_EQ(viewportWidth, pc.viewportWidth(viewportHeight));
}

TEST(PerspectiveCamera, canFitViewportDimension)
{
    PerspectiveCamera pc;

    pc.aspectRatio({1920, 1080});

    ASSERT_EQ(glm::uvec2(1920, 1080), pc.fitWiewportDimension({1921, 1080}));
    ASSERT_EQ(glm::uvec2(1920, 1080), pc.fitWiewportDimension({1920, 1090}));
    ASSERT_EQ(glm::uvec2(1921, 1080), pc.fitWiewportDimension({1921, 1091}));
    ASSERT_EQ(glm::uvec2(3840, 2160), pc.fitWiewportDimension({3840, 2160}));
}

TEST(PerspectiveCamera, defaultProjectionMatrix)
{
    PerspectiveCamera pc;

    glm::mat4x4 expected = glm::infinitePerspective(pc.fovy(), pc.aspectRatio(), pc.nearDistance());

    ASSERT_EQ(expected, pc.projectionMatrix());
}

TEST(PerspectiveCamera, projectionMatrixWithFar)
{
    PerspectiveCamera pc;
    pc.farDistance(pc.nearDistance() + 1);

    glm::mat4x4 expected = glm::perspective(pc.fovy(), pc.aspectRatio(), pc.nearDistance(), pc.farDistance());

    ASSERT_EQ(expected, pc.projectionMatrix());
}

TEST(OrthographicCamera, projectionMatrix)
{
    OrthographicCamera pc;
    pc.nearDistance(5);
    pc.farDistance(10);
    pc.width(2);
    pc.aspectRatio(2,1);

    glm::mat4x4 expected = glm::ortho(-1.0f, 1.0f, -.5f, .5f, 5.0f, 10.0f);

    ASSERT_EQ(expected, pc.projectionMatrix());
}
