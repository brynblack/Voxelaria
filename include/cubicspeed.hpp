#ifndef cubicspeedlibs
#define cubicspeedlibs
#include <glm/ext/vector_float3.hpp>
namespace cubicSpeed
{
    class entity {
    public:
        glm::vec3 xyz;
        double pitch;
        double yaw;
        glm::vec3 direction;
        float speed;
    public:
        void set_xyz(glm::vec3);
        void set_pitch(double);
        void set_yaw(double);
        void set_direction(glm::vec3);
        void set_speed(float);
    };
    void entity::set_xyz(glm::vec3 XYZ)
    {
        xyz = XYZ;
    };
    void entity::set_pitch(double p)
    {
        pitch = p;
    };
    void entity::set_yaw(double y)
    {
        yaw = y;
    };
    void entity::set_direction(glm::vec3 d)
    {
        direction = d;
    };
    void entity::set_speed(float s)
    {
        speed = s;
    };
};
#endif