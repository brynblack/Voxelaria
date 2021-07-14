#ifndef cubicspeedlibs
#define cubicspeedlibs
#include <glm/ext/vector_float3.hpp>
namespace cubicSpeed {
    class camera {
    public:
        glm::vec3 xyz, velocity, front, up, direction;
        GLdouble pitch, yaw;
        GLfloat speed, smoothing, jumpHeight, fov;

    public:
        GLvoid set_xyz(glm::vec3 _xyz)
        {
            this->xyz = _xyz;
        };
        GLvoid set_velocity(glm::vec3 _velocity)
        {
            this->velocity = _velocity;
        };
        GLvoid set_pitch(GLdouble _pitch)
        {
            this->pitch = _pitch;
        };

        GLvoid set_yaw(GLdouble _yaw)
        {
            this->yaw = _yaw;
        };

        GLvoid set_front(glm::vec3 _front)
        {
            this->front = _front;
        };

        GLvoid set_up(glm::vec3 _up)
        {
            this->up = _up;
        };

        GLvoid set_direction(glm::vec3 _direction)
        {
            this->direction = _direction;
        };

        GLvoid set_speed(GLfloat _speed)
        {
            this->speed = _speed;
        };
        GLvoid set_smoothing(GLfloat _smoothing)
        {
            this->smoothing = _smoothing;
        };
        GLvoid set_jumpHeight(GLfloat _jumpHeight)
        {
            this->jumpHeight = _jumpHeight;
        };
        GLvoid set_fov(GLfloat _fov)
        {
            this->fov = _fov;
        };
    };
    class mouse {
    public:
        GLdouble lastX, lastY, x_offset, y_offset;
        GLfloat sensitivity;
        GLboolean firstMouse;
    };
}
#endif