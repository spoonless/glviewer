#include "glm/trigonometric.hpp"
#include "glm/gtx/transform.hpp"
#include "Camera.hpp"

glv::Camera::~Camera()
{
}

glv::PerspectiveCamera::PerspectiveCamera() :
    _fovy{glm::radians(70.0f)}, _aspectRatio{1.0f}, _near{.5f}, _far{.0f}
{

}

glm::mat4x4 glv::PerspectiveCamera::projectionMatrix()
{
    if (_far > _near)
    {
        return glm::perspective(_fovy, _aspectRatio, _near, _far);
    }
    else
    {
        return glm::infinitePerspective(_fovy, _aspectRatio, _near);
    }
}

glm::uvec2 glv::PerspectiveCamera::fitWiewportDimension(unsigned int viewportWidth, unsigned int viewportHeight) const
{
    if (viewportHeight * _aspectRatio > viewportWidth)
    {
        return glm::uvec2(viewportWidth, this->viewportHeight(viewportWidth));
    }
    else
    {
        return glm::uvec2(this->viewportWidth(viewportHeight), viewportHeight);
    }
}
