#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "cubicspeed.hpp"
#include <chrono>
#include <string>
#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint vbo;
GLuint ibo;
GLuint shader;

GLint cubeCount = 0;

cubicSpeed::camera camera;
cubicSpeed::mouse mouse;

GLfloat frameDelta;
glm::vec3 positions[100];

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

void mouse_callback(__attribute__((unused)) GLFWwindow* window, double x, double y)
{
    if (mouse.firstMouse)
    {
        mouse.lastX = x;
        mouse.lastY = y;
        mouse.firstMouse = false;
    }

    mouse.x_offset = x - mouse.lastX;
    mouse.y_offset = mouse.lastY - y;
    mouse.lastX = x;
    mouse.lastY = y;
    mouse.x_offset *= mouse.sensitivity;
    mouse.y_offset *= mouse.sensitivity;
    camera.yaw += mouse.x_offset;
    camera.pitch += mouse.y_offset;

    if(camera.pitch > 89.0f)
    {
        camera.pitch = 89.0f;
    }
    if(camera.pitch < -89.0f)
    {
        camera.pitch = -89.0f;
    }

    camera.direction.x = (float)(cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)));
    camera.direction.y = (float)(sin(glm::radians(camera.pitch)));
    camera.direction.z = (float)(sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)));
    camera.front = glm::normalize(camera.direction);
}

bool checkCollision(glm::vec3 xyz)
{
    bool locationInsideBlock = false;
    if (cubeCount != 0)
    {
        for (int i = 0; i < cubeCount; i++)
        {
            if (positions[i] == glm::vec3(std::floor(xyz.x), std::floor(xyz.y), std::floor(xyz.z)))
            {
                locationInsideBlock = true;
            }
        }
    }
    return locationInsideBlock;
}

void createNewVoxel(glm::vec3 xyz)
{
    if (!checkCollision(xyz))
    {
        cubeCount++;
        positions[cubeCount - 1] = glm::vec3(std::floor(xyz.x), std::floor(xyz.y), std::floor(xyz.z));
        GLfloat vertices[48] = {
                std::floor(xyz.x+1.0f), std::floor(xyz.y+1.0f), std::floor(xyz.z+0.0f), 1.0f, 0.0f, 0.0f,
                std::floor(xyz.x+1.0f), std::floor(xyz.y+0.0f), std::floor(xyz.z+0.0f), 0.0f, 1.0f, 0.0f,
                std::floor(xyz.x+0.0f), std::floor(xyz.y+0.0f), std::floor(xyz.z+0.0f), 0.0f, 0.0f, 1.0f,
                std::floor(xyz.x+0.0f), std::floor(xyz.y+1.0f), std::floor(xyz.z+0.0f), 0.0f, 0.0f, 0.0f,
                std::floor(xyz.x+1.0f), std::floor(xyz.y+1.0f), std::floor(xyz.z+1.0f), 0.0f, 1.0f, 0.0f,
                std::floor(xyz.x+0.0f), std::floor(xyz.y+1.0f), std::floor(xyz.z+1.0f), 0.0f, 0.0f, 1.0f,
                std::floor(xyz.x+1.0f), std::floor(xyz.y+0.0f), std::floor(xyz.z+1.0f), 0.0f, 0.0f, 0.0f,
                std::floor(xyz.x+0.0f), std::floor(xyz.y+0.0f), std::floor(xyz.z+1.0f), 0.0f, 0.0f, 1.0f
        };
        GLint indices[36] = {
                (cubeCount - 1) * 8 + 0, (cubeCount - 1) * 8 + 1, (cubeCount - 1) * 8 + 2,
                (cubeCount - 1) * 8 + 2, (cubeCount - 1) * 8 + 3, (cubeCount - 1) * 8 + 0,
                (cubeCount - 1) * 8 + 4, (cubeCount - 1) * 8 + 0, (cubeCount - 1) * 8 + 3,
                (cubeCount - 1) * 8 + 3, (cubeCount - 1) * 8 + 5, (cubeCount - 1) * 8 + 4,
                (cubeCount - 1) * 8 + 4, (cubeCount - 1) * 8 + 6, (cubeCount - 1) * 8 + 1,
                (cubeCount - 1) * 8 + 1, (cubeCount - 1) * 8 + 0, (cubeCount - 1) * 8 + 4,
                (cubeCount - 1) * 8 + 5, (cubeCount - 1) * 8 + 7, (cubeCount - 1) * 8 + 6,
                (cubeCount - 1) * 8 + 6, (cubeCount - 1) * 8 + 4, (cubeCount - 1) * 8 + 5,
                (cubeCount - 1) * 8 + 3, (cubeCount - 1) * 8 + 2, (cubeCount - 1) * 8 + 7,
                (cubeCount - 1) * 8 + 7, (cubeCount - 1) * 8 + 5, (cubeCount - 1) * 8 + 3,
                (cubeCount - 1) * 8 + 1, (cubeCount - 1) * 8 + 6, (cubeCount - 1) * 8 + 7,
                (cubeCount - 1) * 8 + 7, (cubeCount - 1) * 8 + 2, (cubeCount - 1) * 8 + 1
        };
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, (cubeCount - 1) * 48 * (GLintptr)sizeof(GLfloat), 48 * sizeof(GLfloat), vertices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (cubeCount - 1) * 36 * (GLintptr)sizeof(GLint), 36 * sizeof(GLint), indices);
    }
}

void deleteVoxel(glm::vec3 xyz)
{
    if (cubeCount != 0)
    {
        for (int i = 0; i < cubeCount; i++)
        {
            if (positions[i] == glm::vec3(std::floor(xyz.x), std::floor(xyz.y), std::floor(xyz.z)))
            {
                for(int ind = i; ind <= 100-1; ind++)
                {
                    positions[i] = positions[ind+1];
                }
                positions[100-1] = { };
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                glClearBufferSubData(GL_ARRAY_BUFFER, GL_R32F, i * 48 * (GLintptr)sizeof(GLfloat), 48 * sizeof(GLfloat), GL_RED, GL_FLOAT, nullptr);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glClearBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_R32F, i * 36 * (GLintptr)sizeof(GLfloat), 36 * sizeof(GLfloat), GL_RED, GL_INT, nullptr);
            }
        }
    }
}

void mouse_button_callback(__attribute__((unused)) GLFWwindow* window, int button, int action, __attribute__((unused)) int mods)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        createNewVoxel(glm::vec3(camera.xyz.x, camera.xyz.y - 2, camera.xyz.z));
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        deleteVoxel(glm::vec3(camera.xyz.x, camera.xyz.y - 2, camera.xyz.z));
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cout << "A fatal error has occurred: Unable to initialise GLFW" << std::endl;
        return -1;
    }
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    auto* mode = (GLFWvidmode*)glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "CubicSpeed", monitor, nullptr);
    if (!window)
    {
        std::cout << "A fatal error has occurred: Unable to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK)
    {
        std::cout << "A fatal error has occurred: Unable to initialise GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 10000000, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10000000, nullptr, GL_DYNAMIC_DRAW);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::string vertexShader = "#version 330 core\n"
                               "layout (location = 0) in vec3 v_pos;\n"
                               "layout (location = 1) in vec3 v_color;\n"
                               "uniform mat4 model;\n"
                               "uniform mat4 view;\n"
                               "uniform mat4 projection;\n"
                               "out vec4 vs_color;\n"
                               "void main()\n"
                               "{\n"
                               "	gl_Position = projection * view * model * vec4(v_pos, 1.0);\n"
                               "	vs_color = vec4(v_color, 1.0);\n"
                               "}";
    std::string fragmentShader = "#version 330 core\n"
                                 "in vec4 vs_color;\n"
                                 "out vec4 fs_color;\n"
                                 "void main()\n"
                                 "{\n"
                                 "	fs_color = vs_color;\n"
                                 "}";
    shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    camera.set_xyz(glm::vec3(0.0f, 0.0f,  0.0f));
    camera.set_front(glm::vec3(0.0f, 0.0f, -1.0f));
    camera.set_up(glm::vec3(0.0f, 1.0f, 0.0f));
    camera.set_pitch(0.0);
    camera.set_yaw(-90.0);
    camera.set_fov(90.0f);
    camera.set_speed(0.01f);
    camera.set_smoothing(0.005f);

    mouse.lastX = mode->width / 2.0;
    mouse.lastY = mode->height / 2.0;
    mouse.sensitivity = 0.1f;
    mouse.firstMouse = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 View = glm::mat4(1.0f);
    glm::mat4 Projection = glm::perspective(glm::radians(camera.fov), (GLfloat)mode->width/(GLfloat)mode->height, 0.1f, 100.0f);

    GLint model_location = glGetUniformLocation(shader, "model");
    GLint view_location = glGetUniformLocation(shader, "view");
    GLint projection_location = glGetUniformLocation(shader, "projection");

    std::chrono::high_resolution_clock::time_point t1, t2;

    while (!glfwWindowShouldClose(window))
    {
        frameDelta = std::chrono::duration<float, std::milli>(t2 - t1).count();
        t1 = std::chrono::high_resolution_clock::now();

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            break;
        }
        if (!checkCollision(camera.xyz))
        {
            if (glfwGetKey(window, GLFW_KEY_A))
            {
                camera.velocity -= glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * frameDelta;
            }
            if (glfwGetKey(window, GLFW_KEY_D))
            {
                camera.velocity += glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * frameDelta;
            }
            if (glfwGetKey(window, GLFW_KEY_S))
            {
                camera.velocity -= camera.speed * glm::vec3(cos(glm::radians(camera.yaw)), 0.0f, sin(glm::radians(camera.yaw))) * glm::vec3(1.0f, 0.0f, 1.0f) * frameDelta;
            }
            if (glfwGetKey(window, GLFW_KEY_W))
            {
                camera.velocity += camera.speed * glm::vec3(cos(glm::radians(camera.yaw)), 0.0f, sin(glm::radians(camera.yaw))) * glm::vec3(1.0f, 0.0f, 1.0f) * frameDelta;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
            {
                camera.velocity -= camera.speed * camera.up * frameDelta;
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE))
            {
                camera.velocity += camera.speed * camera.up * frameDelta;
            }
        }

        if (checkCollision(camera.xyz))
        {
            camera.velocity *= -2.0f;
        }

        camera.velocity -= camera.velocity * camera.smoothing * frameDelta;

        camera.xyz += camera.velocity * camera.speed * frameDelta;

        View = glm::lookAt(camera.xyz, camera.xyz + camera.front, camera.up);

        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(Model));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(Projection));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, (GLsizei)cubeCount * 36, GL_UNSIGNED_INT, nullptr);
        glfwSwapBuffers(window);

        t2 = std::chrono::high_resolution_clock::now();
    }
    glfwTerminate();
    return 0;
}