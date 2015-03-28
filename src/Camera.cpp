#include "glm/trigonometric.hpp"
#include "glm/gtx/transform.hpp"
#include "Camera.hpp"

glv::Viewport::Viewport(): _width{100}, _height(100)
{

}

glv::Camera::Camera():
    _near{.5f}, _far{.0f}
{
}

glv::Camera::~Camera()
{
}

glv::OrthographicCamera::OrthographicCamera(): _width{1.0f}
{
    if (_near >= _far)
    {
        _far = _near * 1000.0f;
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
        return glm::perspective(_fovy, _viewport.aspectRatio(), _near, _far);
    }
    else
    {
        return glm::infinitePerspective(_fovy, _viewport.aspectRatio(), _near);
    }
}

