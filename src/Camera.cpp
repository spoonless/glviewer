#include "glm/trigonometric.hpp"
#include "glm/gtx/transform.hpp"
#include "Camera.hpp"

glv::Camera::Camera():
    _aspectRatio{1.0f}, _near{.5f}, _far{.0f}
{
}

glv::Camera::~Camera()
{
}

glm::uvec2 glv::Camera::fitWiewportDimension(unsigned int viewportWidth, unsigned int viewportHeight) const
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

glv::OrthographicCamera::OrthographicCamera(): _width{1.0f}
{
    if (_near >= _far)
    {
        _far = _near * 1000.0;
    }
}

glm::mat4x4 glv::OrthographicCamera::projectionMatrix() const
{
    return glm::ortho(left(), right(), bottom(), top(), _near, _far);
}

glv::PerspectiveCamera::PerspectiveCamera(): _fovy{glm::radians(70.0f)}
{

}

glm::mat4x4 glv::PerspectiveCamera::projectionMatrix() const
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

