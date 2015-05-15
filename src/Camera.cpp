#include "glm/trigonometric.hpp"
#include "glm/gtx/transform.hpp"
#include "Camera.hpp"

ogl::Viewport::Viewport(): _width{100}, _height{100}
{

}

ogl::Camera::Camera():
    _near{.5f}, _far{.0f}
{
}

ogl::Camera::~Camera()
{
}

ogl::OrthographicCamera::OrthographicCamera(float width): _width{width}
{
    if (_near >= _far)
    {
        _far = _near * 1000.0f;
    }
}

glm::mat4x4 ogl::OrthographicCamera::projectionMatrix() const
{
    return glm::ortho(left(), right(), bottom(), top(), _near, _far);
}

ogl::PerspectiveCamera::PerspectiveCamera(): _fovy{glm::radians(70.0f)}
{

}

ogl::PerspectiveCamera::PerspectiveCamera(float fovy): _fovy{fovy}
{

}

glm::mat4x4 ogl::PerspectiveCamera::projectionMatrix() const
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

