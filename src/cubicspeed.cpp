#include "cubicspeed.hpp"
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <chrono>
#include <string>
#include <iostream>

GLFWmonitor* monitor;
GLFWvidmode* mode;
GLFWwindow* window;

cubicSpeed::entity player, enemy, goal;
cubicSpeed::properties GLwindow;

std::chrono::high_resolution_clock::time_point t1, t2;

int score;
float frameDelta;

unsigned int buffer;
unsigned int shader;

void config()
{
	player.set_size(16.0f);
	player.set_rgba(1.0f, 1.0f, 0.0f, 1.0f);
	player.set_speed(1.0f);
	player.set_xy(0.0f, 0.0f);

	enemy.set_size(16.0f);
	enemy.set_rgba(1.0f, 0.0f, 0.0f, 1.0f);
	enemy.set_speed(0.1f);
	enemy.set_xy((float)GLwindow.width, (float)GLwindow.height);

	goal.set_size(64.0f);
	goal.set_rgba(0.0f, 1.0f, 0.0f, 1.0f);
	goal.set_xy(float(rand() % GLwindow.width), float(rand() % GLwindow.height));
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str() ;
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

int init()
{
	if (!glfwInit())
		return -1;
	monitor = glfwGetPrimaryMonitor();
	mode = (GLFWvidmode*)glfwGetVideoMode(monitor);
	GLwindow.width = mode->width;
	GLwindow.height = mode->height;
	window = glfwCreateWindow(GLwindow.width, GLwindow.height, "OpenGL", monitor, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (GLvoid*)(6 * sizeof(float)));
	glEnable(GL_PROGRAM_POINT_SIZE);
	std::string vertexShader = "#version 330 core\n"
		"layout (location = 0) in vec2 position;\n"
		"layout (location = 1) in vec4 color;\n"
		"layout (location = 2) in float size;\n"
		"uniform mat4 projection;\n"
		"out vec4 objColor;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = projection * vec4(position, 0.0, 1.0);\n"
		"	objColor = color;\n"
		"	gl_PointSize = size;\n"
		"}\0";
	std::string fragmentShader = "#version 330 core\n"
		"in vec4 objColor;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"	color = objColor;\n"
		"}\0";
	shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);
	return 0;
}

int input()
{
	glfwPollEvents();
	if (glfwGetKey(window, GLFW_KEY_A) && !(player.xy[0] < 0.0f))
		player.xy[0] -= player.speed * frameDelta;
	if (glfwGetKey(window, GLFW_KEY_D) && !(player.xy[0] > (float)GLwindow.width))
		player.xy[0] += player.speed * frameDelta;
	if (glfwGetKey(window, GLFW_KEY_S) && !(player.xy[1] < 0.0f))
		player.xy[1] -= player.speed * frameDelta;
	if (glfwGetKey(window, GLFW_KEY_W) && !(player.xy[1] > (float)GLwindow.height))
		player.xy[1] += player.speed * frameDelta;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glfwTerminate();
		return 1;
	}
	return 0;
}

void calculate()
{
	enemy.dxy[0] = player.xy[0] - enemy.xy[0];
	enemy.dxy[1] = player.xy[1] - enemy.xy[1];
	enemy.dist = sqrt((enemy.dxy[0] * enemy.dxy[0]) + (enemy.dxy[1] * enemy.dxy[1]));
	enemy.dxy[0] /= enemy.dist;
	enemy.dxy[1] /= enemy.dist;
	enemy.xy[0] += enemy.dxy[0] * enemy.speed * frameDelta;
	enemy.xy[1] += enemy.dxy[1] * enemy.speed * frameDelta;
	goal.dxy[0] = player.xy[0] - goal.xy[0];
	goal.dxy[1] = player.xy[1] - goal.xy[1];
	goal.dist = sqrt((goal.dxy[0] * goal.dxy[0]) + (goal.dxy[1] * goal.dxy[1]));
	if (enemy.dist < ((player.size + enemy.size) / 2.0f))
	{
		config();
		score = 0;
	}
	if (goal.dist < ((player.size + goal.size) / 2.0f))
	{
		score += 1;
		goal.set_xy(float(rand() % GLwindow.width), float(rand() % GLwindow.height));
		enemy.set_speed(enemy.speed + 0.1f);
		enemy.set_size(enemy.size + 0.1f);
	}
}

void draw(float *matrix, int transform_location)
{
	float verticies[21] = {
		player.xy[0], player.xy[1], player.rgba[0], player.rgba[1], player.rgba[2], player.rgba[3], player.size,
		enemy.xy[0], enemy.xy[1], enemy.rgba[0], enemy.rgba[1], enemy.rgba[2], enemy.rgba[3], enemy.size,
		goal.xy[0], goal.xy[1], goal.rgba[0], goal.rgba[1], goal.rgba[2], goal.rgba[3], goal.size
	};
	glUniformMatrix4fv(transform_location, 1, GL_FALSE, matrix);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 21 * sizeof(float), verticies, GL_DYNAMIC_DRAW);
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_POINTS, 0, 3);
	glFlush();
	glfwSwapBuffers(window);
}

int main()
{
	if (init() == -1)
		return -1;
	config();
	float *matrix = cubicSpeed::ortho(0.0f, (float)GLwindow.width, 0.0f, (float)GLwindow.height, 0.0f, 1.0f);
	int transform_location = glGetUniformLocation(shader, "projection");
	while (!glfwWindowShouldClose(window))
	{
		frameDelta = std::chrono::duration<float, std::milli>(t2 - t1).count(); // Game speed modifier
		t1 = std::chrono::high_resolution_clock::now();
		if (input() == 1)
			return 0;
		calculate();
		draw(matrix, transform_location);
		t2 = std::chrono::high_resolution_clock::now();
	}
	glfwTerminate();
	return 0;
}