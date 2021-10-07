#ifndef cubicspeedlibs
#define cubicspeedlibs
#include <glm/ext/vector_float3.hpp>
namespace cubicSpeed {
    class camera {
    public:
        glm::vec3 xyz, velocity, front, up, direction;
        double pitch, yaw;
        float speed, smoothing, jumpHeight, fov;
        bool flying;

    public:
        void set_xyz(glm::vec3 _xyz)
        {
            this->xyz = _xyz;
        };
        void set_velocity(glm::vec3 _velocity)
        {
            this->velocity = _velocity;
        };
        void set_pitch(double _pitch)
        {
            this->pitch = _pitch;
        };

        void set_yaw(double _yaw)
        {
            this->yaw = _yaw;
        };

        void set_front(glm::vec3 _front)
        {
            this->front = _front;
        };

        void set_up(glm::vec3 _up)
        {
            this->up = _up;
        };

        void set_direction(glm::vec3 _direction)
        {
            this->direction = _direction;
        };

        void set_speed(float _speed)
        {
            this->speed = _speed;
        };
        void set_smoothing(float _smoothing)
        {
            this->smoothing = _smoothing;
        };
        void set_jumpHeight(float _jumpHeight)
        {
            this->jumpHeight = _jumpHeight;
        };
        void set_fov(float _fov)
        {
            this->fov = _fov;
        };
        void enable_flying(bool _flying)
        {
            this->flying = _flying;
        };
    };
    class mouse {
    public:
        double lastX, lastY, x_offset, y_offset;
        float sensitivity;
        bool firstMouse;
    };
}
#endif