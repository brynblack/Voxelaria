// OpenGL Headers
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

// Utility Headers
#include "cubicspeed.hpp"
#include <chrono>
#include <string>
#include <iostream>

// GLM Headers
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

struct properties {
    int width, height;
    const char *title;
};

GLFWmonitor* monitor;
GLFWvidmode* mode;
GLFWwindow* window;

cubicSpeed::entity camera;
properties GLwindow;

glm::vec3 cameraFront, cameraUp;

double lastX = GLwindow.width / 2.0;
double lastY = GLwindow.height / 2.0;
float fov = 45.0f;
bool firstMouse = true;

float frameDelta;

unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
};

unsigned int buffer;
unsigned int ibo;
unsigned int shader;

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

void mouse_callback(GLFWwindow* wwindow, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.yaw   += xoffset;
    camera.pitch += yoffset;

    if(camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if(camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    camera.direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.direction.y = sin(glm::radians(camera.pitch));
    camera.direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    cameraFront = glm::normalize(camera.direction);
}

int main()
{
    std::chrono::high_resolution_clock::time_point t1, t2; // Define time point variables

    // Init glfw
    if (!glfwInit())
        return -1;
    monitor = glfwGetPrimaryMonitor();
    mode = (GLFWvidmode*)glfwGetVideoMode(monitor);
    GLwindow.width = mode->width;
    GLwindow.height = mode->height;
    window = glfwCreateWindow(GLwindow.width, GLwindow.height, "CubicSpeed", monitor, nullptr);
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

    // Configure mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float)));

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

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

    // Set camera settings
    camera.set_xyz(glm::vec3(0.0f, 0.0f,  3.0f));
    camera.set_pitch(0.0);
    camera.set_yaw(-90.0);
    camera.set_speed(0.01f);

    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

    // Declare matrices
    glm::mat4 Model = glm::mat4(1.0f);
    //Model = glm::rotate(Model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 View = glm::mat4(1.0f);
    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), (GLfloat)GLwindow.width/(GLfloat)GLwindow.height, 0.1f, 100.0f);

    // Get matrix locations
    int model_location = glGetUniformLocation(shader, "model");
    int view_location = glGetUniformLocation(shader, "view");
    int projection_location = glGetUniformLocation(shader, "projection");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate time delta
        frameDelta = std::chrono::duration<float, std::milli>(t2 - t1).count(); // Game speed modifier
        t1 = std::chrono::high_resolution_clock::now(); // Get first time point

        // Input stuff
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_A))
            camera.xyz -= glm::normalize(glm::cross(cameraFront, cameraUp)) * camera.speed * frameDelta;
        if (glfwGetKey(window, GLFW_KEY_D))
            camera.xyz += glm::normalize(glm::cross(cameraFront, cameraUp)) * camera.speed * frameDelta;
        if (glfwGetKey(window, GLFW_KEY_S))
            camera.xyz -= camera.speed * cameraFront * frameDelta;
        if (glfwGetKey(window, GLFW_KEY_W))
            camera.xyz += camera.speed * cameraFront * frameDelta;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwTerminate();
            return 0;
        }

        // Draw everything
        float verticies[24] = {
                0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
                -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f
        };

        View = glm::lookAt(camera.xyz, camera.xyz + cameraFront, cameraUp);

        // Send matrices to shader
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(Model));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(Projection));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(float), verticies, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_DYNAMIC_DRAW);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glFlush();
        glfwSwapBuffers(window);

        t2 = std::chrono::high_resolution_clock::now(); // Get second time point
    }
    glfwTerminate();
    return 0;
}