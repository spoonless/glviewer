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

class Viewport
{
public:
    Viewport();

    template<typename T>
    operator glm::tvec2<T> () const
    {
        return glm::tvec2<T>(static_cast<T>(_width), static_cast<T>(_height));
    }

    unsigned int width() const
    {
        return _width;
    }

    unsigned int height() const
    {
        return _height;
    }

    float aspectRatio() const
    {
        return static_cast<float>(_width) / static_cast<float>(_height);
    }

    void set(unsigned int width, unsigned int height)
    {
        assert (width > 0);
        assert (height > 0);
        _width = width;
        _height = height;
    }

private:
    unsigned int _width;
    unsigned int _height;
};

class Camera : public WorldObject
{
public:
    Camera();
    virtual ~Camera();

    virtual glm::mat4x4 projectionMatrix() const = 0;

    Viewport & viewport()
    {
        return _viewport;
    }

    const Viewport & viewport() const
    {
        return _viewport;
    }

    inline void nearDistance(float d)
    {
        assert(d >= .0f);
        _near = d;
    }

    inline float nearDistance() const
    {
        return _near;
    }

    inline void farDistance(float d)
    {
        assert(d >= .0f);
        _far = d;
    }

    inline float farDistance() const
    {
        return _far;
    }

protected:
    float _near;
    float _far;
    Viewport _viewport;
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
        return _width / (2.0f * _viewport.aspectRatio());
    }

    inline float bottom() const
    {
        return - _width / (2.0f * _viewport.aspectRatio());
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
