#include <gtest/gtest.h>
#include "glm/gtx/transform.hpp"
#include "Camera.hpp"

using namespace glv;

TEST(PerspectiveCamera, defaultProjectionMatrix)
{
    PerspectiveCamera pc;

    glm::mat4x4 expected = glm::infinitePerspective(pc.fovy(), pc.viewport().aspectRatio(), pc.nearDistance());

    ASSERT_EQ(expected, pc.projectionMatrix());
}

TEST(PerspectiveCamera, projectionMatrixWithFar)
{
    PerspectiveCamera pc;
    pc.farDistance(pc.nearDistance() + 1);

    glm::mat4x4 expected = glm::perspective(pc.fovy(), pc.viewport().aspectRatio(), pc.nearDistance(), pc.farDistance());

    ASSERT_EQ(expected, pc.projectionMatrix());
}

TEST(OrthographicCamera, projectionMatrix)
{
    OrthographicCamera pc;
    pc.nearDistance(5);
    pc.farDistance(10);
    pc.width(2);
    pc.viewport().set(200,100);

    glm::mat4x4 expected = glm::ortho(-1.0f, 1.0f, -.5f, .5f, 5.0f, 10.0f);

    ASSERT_EQ(expected, pc.projectionMatrix());
}
