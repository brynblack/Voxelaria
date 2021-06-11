#ifndef cubicspeedlibs
#define cubicspeedlibs
#include <glm/ext/vector_float3.hpp>
namespace cubicSpeed {
    class camera {
    public:
        glm::vec3 xyz, velocity, front, up, direction;
        double pitch, yaw;
        float speed, smoothing, fov;
    public:
        void set_xyz(glm::vec3 xyz)
        {
            this->xyz = xyz;
        };
        void set_velocity(glm::vec3 velocity)
        {
            this->velocity = velocity;
        };
        void set_pitch(double pitch)
        {
            this->pitch = pitch;
        };

        void set_yaw(double yaw)
        {
            this->yaw = yaw;
        };

        void set_front(glm::vec3 front)
        {
            this->front = front;
        };

        void set_up(glm::vec3 up)
        {
            this->up = up;
        };

        void set_direction(glm::vec3 direction)
        {
            this->direction = direction;
        };

        void set_speed(float speed)
        {
            this->speed = speed;
        };
        void set_smoothing(float smoothing)
        {
            this->smoothing = smoothing;
        };
        void set_fov(float fov)
        {
            this->fov = fov;
        };
    };
    class window {
    public:
        int width, height;
        const char *title;
    public:
        void set_width(int width)
        {
            this->width = width;
        };
        void set_height(int height)
        {
            this->height = height;
        };
        void set_title(const char* title)
        {
            this->title = title;
        };
    };
    class mouse {
    public:
        double lastX, lastY, xoffset, yoffset;
        float sensitivity;
        bool firstMouse;
    };
}
#endif