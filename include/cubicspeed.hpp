#ifndef cubicspeedlibs
#define cubicspeedlibs
namespace cubicSpeed
{
	class entity {
	public:
		float size;
		float rgba[4];
		float speed;
		float xy[2];
		float dxy[2];
		float dist;
	public:
		void set_size(float);
		void set_rgba(float, float, float, float);
		void set_xy(float, float);
		void set_speed(float);
	};
	void entity::set_size(float s)
	{
		size = s;
	};
	void entity::set_rgba(float r, float g, float b, float a)
	{
		rgba[0] = r;
		rgba[1] = g;
		rgba[2] = b;
		rgba[3] = a;
	};
	void entity::set_speed(float s)
	{
		speed = s;
	};
	void entity::set_xy(float x, float y)
	{
		xy[0] = x;
		xy[1] = y;
	};
	float *ortho(float left, float right, float bottom, float top, float near, float far)
	{
		static float matrix[4][4];
		matrix[0][0] = 2.0f / (right - left);
		matrix[1][1] = 2.0f / (top - bottom);
		matrix[2][2] = -2.0f / (far - near);
		matrix[3][0] = -(right + left) / (right - left);
		matrix[3][1] = -(top + bottom) / (top - bottom);
		matrix[3][2] = -(far + near) / (far - near);
		matrix[3][3] = 1.0f;
		return *matrix;
	};
	struct properties {
		int width, height;
		const char *title;
	};
};
#endif