#ifndef CAMERA_H
#define CAMERA_H

#include <assert.h>
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"

namespace glv
{

/****************************************************************/
// World object
/****************************************************************/

class WorldObject
{
public:
    WorldObject() = default;
    WorldObject(const WorldObject &) = delete;
    WorldObject & operator = (const WorldObject &) = delete;
};


/****************************************************************/
// Cameras
/****************************************************************/

class Camera : public WorldObject
{
public:
    Camera();
    virtual ~Camera();

    virtual glm::mat4x4 projectionMatrix() const = 0;

    inline void aspectRatio(glm::uvec2 dimension)
    {
        aspectRatio(dimension.x, dimension.y);
    }

    inline float aspectRatio() const
    {
        return _aspectRatio;
    }

    inline void aspectRatio(unsigned int viewportWidth, unsigned int viewportHeight)
    {
        assert(viewportWidth > 0);
        assert(viewportHeight > 0);
        _aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
    }

    unsigned int viewportWidth (unsigned int viewportHeight) const
    {
        return static_cast<unsigned int>(viewportHeight * _aspectRatio);
    }

    unsigned int viewportHeight(unsigned int viewportWidth) const
    {
        return static_cast<unsigned int>(viewportWidth / _aspectRatio);
    }

    inline glm::uvec2 fitWiewportDimension(glm::uvec2 dimension)
    {
        return fitWiewportDimension(dimension.x, dimension.y);
    }

    glm::uvec2 fitWiewportDimension(unsigned int viewportWidth, unsigned int viewportHeight) const;

    inline void near(float near)
    {
        assert(near >= .0f);
        _near = near;
    }

    inline float near() const
    {
        return _near;
    }

    inline void far(float far)
    {
        assert(far >= .0f);
        _far = far;
    }

    inline float far() const
    {
        return _far;
    }

protected:
    float _aspectRatio;
    float _near;
    float _far;
};

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera();

    virtual glm::mat4x4 projectionMatrix() const;

    inline void width(float width)
    {
        assert(width > .0f);
        _width = width;
    }

    inline float width() const
    {
        return _width;
    }

    inline float right() const
    {
        return _width / 2.0f;
    }

    inline float left() const
    {
        return - _width / 2.0f;
    }

    inline float top() const
    {
        return _width / (2.0f * _aspectRatio);
    }

    inline float bottom() const
    {
        return - _width / (2.0f * _aspectRatio);
    }

private:
    float _width;
};

class PerspectiveCamera : public Camera
{
public:

    PerspectiveCamera();

    virtual glm::mat4x4 projectionMatrix() const;

    inline void fovy(float radians)
    {
        assert(radians > .0f);
        _fovy = radians;
    }

    inline float fovy() const
    {
        return _fovy;
    }


private:
    float _fovy;
};

}

#endif

