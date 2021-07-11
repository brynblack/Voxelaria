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

// TODO: Add collision detection system
// TODO: Add in basic physics engine
// TODO: Add in voxel like coordinate system
// TODO: Add OBAMA Texture

// Declare GLFW variables
GLFWmonitor* monitor;
GLFWvidmode* mode;
GLFWwindow* window;

// Declare camera, window and mouse instances
cubicSpeed::window GLwindow;
cubicSpeed::camera camera;
cubicSpeed::mouse mouse;

// Declare frameDelta variable used to calculate time delta
float frameDelta;

// Define buffer variables
unsigned int vbo;
unsigned int ibo;
unsigned int shader;

unsigned int cubeCount = 0;

// Compile shader function to compile the given shader code
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

// Create shader function to generate a shader
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

// Function to process mouse input
void mouse_callback(__attribute__((unused)) GLFWwindow* wwindow, double xpos, double ypos)
{
    if (mouse.firstMouse)
    {
        mouse.lastX = xpos;
        mouse.lastY = ypos;
        mouse.firstMouse = false;
    }

    mouse.xoffset = xpos - mouse.lastX;
    mouse.yoffset = mouse.lastY - ypos;
    mouse.lastX = xpos;
    mouse.lastY = ypos;

    mouse.xoffset *= mouse.sensitivity;
    mouse.yoffset *= mouse.sensitivity;

    camera.yaw += mouse.xoffset;
    camera.pitch += mouse.yoffset;

    // Lock camera angles
    if(camera.pitch > 89.0f)
    {
        camera.pitch = 89.0f;
    }
    if(camera.pitch < -89.0f)
    {
        camera.pitch = -89.0f;
    }

    // Set camera direction based upon yaw and pitch
    camera.direction.x = (float)(cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)));
    camera.direction.y = (float)(sin(glm::radians(camera.pitch)));
    camera.direction.z = (float)(sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)));
    camera.front = glm::normalize(camera.direction);
}

void addNewCube(float x, float y, float z)
{
    cubeCount++;
    float vertices[48] = {
            std::round(x+1.0f), std::round(y+1.0f), std::round(z+0.0f), 1.0f, 0.0f, 0.0f, // 0
            std::round(x+1.0f), std::round(y+0.0f), std::round(z+0.0f), 0.0f, 1.0f, 0.0f, // 1
            std::round(x+0.0f), std::round(y+0.0f), std::round(z+0.0f), 0.0f, 0.0f, 1.0f, // 2
            std::round(x+0.0f), std::round(y+1.0f), std::round(z+0.0f), 0.0f, 0.0f, 0.0f, // 3
            std::round(x+1.0f), std::round(y+1.0f), std::round(z+-1.0f), 0.0f, 1.0f, 0.0f, // 4
            std::round(x+0.0f), std::round(y+1.0f), std::round(z+-1.0f), 0.0f, 0.0f, 1.0f, // 5
            std::round(x+1.0f), std::round(y+0.0f), std::round(z+-1.0f), 0.0f, 0.0f, 0.0f, // 6
            std::round(x+0.0f), std::round(y+0.0f), std::round(z+-1.0f), 0.0f, 0.0f, 1.0f // 7
    };
    unsigned int indices[36] = {
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind vertex buffer
    glBufferSubData(GL_ARRAY_BUFFER, (cubeCount - 1) * 48 * sizeof(float), 48 * sizeof(float), vertices); // Send vertex data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Bind element buffer
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (cubeCount - 1) * 36 * sizeof(unsigned int), 36 * sizeof(unsigned int), indices); // Send element data
}

// Main function to initialise OpenGL and run all other code
int main()
{
    // GLFW initialisation

    if (glfwInit() == GLFW_FALSE) // Attempt to initialise GLFW
    {
        return -1; // Return -1 if glfwInit() failed
    }
    monitor = glfwGetPrimaryMonitor(); // Get primary monitor
    mode = (GLFWvidmode*)glfwGetVideoMode(monitor); // Get video mode
    GLwindow.set_width(mode->width); // Get window width
    GLwindow.set_height(mode->height); // Get window height
    GLwindow.set_title("CubicSpeed"); // Set window title
    window = glfwCreateWindow(GLwindow.width, GLwindow.height, GLwindow.title, monitor, nullptr); // Create window
    if (!window) // Check if window creation succeeded
    {
        glfwTerminate(); // Terminate GLFW if window creation failed
        return -1; // Return -1 after terminating GLFW
    }
    glfwMakeContextCurrent(window); // Set GLFW context to the current window
    glfwSwapInterval(0); // Disable vsync
    if (glewInit() != GLEW_OK) // Attempt to initialise GLEW
    {
        glfwTerminate(); // Terminate GLFW if GLEW initialisation failed
        return -1; // Return -1 after terminating GLFW
    }

    // Configure settings

    // Set camera settings
    camera.set_fov(90.0f); // Set camera FOV
    camera.set_xyz(glm::vec3(0.5f, 0.5f,  3.0f)); // Set initial camera position
    camera.set_pitch(0.0); // Set initial camera pitch
    camera.set_yaw(-90.0); // Set initial camera yaw
    camera.set_speed(0.01f); // Set camera speed
    camera.set_smoothing(0.01f); // Set camera smoothing value
    camera.front = glm::vec3(0.0f, 0.0f, -1.0f); // Set camera front
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f); // Set camera up

    // Set mouse settings
    mouse.lastX = GLwindow.width / 2.0; // Set initial X mouse position
    mouse.lastY = GLwindow.height / 2.0; // Set initial Y mouse position
    mouse.sensitivity = 0.1f; // Set mouse sensitivity
    mouse.firstMouse = true; // Set firstMouse boolean to true
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Set GLFW input mode
    glfwSetCursorPosCallback(window, mouse_callback); // Set GLFW mouse callback function

    // Generate buffers and define vertex attributes

    // Define element buffer array

    // Array buffer
    glGenBuffers(1, &vbo); // Generate array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind vertex buffer
    glBufferData(GL_ARRAY_BUFFER, 10000000, nullptr, GL_DYNAMIC_DRAW); // Initialise buffer with 10000 bytes of data
    glEnableVertexAttribArray(0); // Enable vertex attribute 1
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)nullptr); // Configure vertex attribute 1
    glEnableVertexAttribArray(1); // Enable vertex attribute 2
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (GLvoid*)(3 * sizeof(float))); // Configure vertex attribute 2

    // Element buffer
    glGenBuffers(1, &ibo); // Generate element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); // Bind element buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10000000, nullptr, GL_DYNAMIC_DRAW); // Initialise buffer with 10000 bytes of data

    // Enable and configure OpenGL functions

    glEnable(GL_BLEND); // Enable OpenGL blending
    glEnable(GL_DEPTH_TEST); // Enable OpenGL depth testing
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set OpenGl blending function

    // Vertex and fragment shader
    // TODO: Add code that allows shader code to be imported from an external file

    // Vertex shader code
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

    // Fragment shader code
    std::string fragmentShader = "#version 330 core\n"
                                 "in vec4 vs_color;\n"
                                 "out vec4 fs_color;\n"
                                 "void main()\n"
                                 "{\n"
                                 "	fs_color = vs_color;\n"
                                 "}";

    // Create shader program
    shader = CreateShader(vertexShader, fragmentShader); // Create shader from vertex and fragment shader code
    glUseProgram(shader); // Tell OpenGL to use the shader

    std::chrono::high_resolution_clock::time_point t1, t2; // Define time point variables

    // Create matrices

    glm::mat4 Model = glm::mat4(1.0f); // Model matrix
    glm::mat4 View = glm::mat4(1.0f); // View matrix
    glm::mat4 Projection = glm::perspective(glm::radians(camera.fov), (GLfloat)GLwindow.width/(GLfloat)GLwindow.height, 0.1f, 100.0f); // Projection matrix

    // Get matrix uniform locations
    int model_location = glGetUniformLocation(shader, "model"); // Model location
    int view_location = glGetUniformLocation(shader, "view"); // View location
    int projection_location = glGetUniformLocation(shader, "projection"); // Projection location

    // Main program loop

    while (!glfwWindowShouldClose(window)) // Loop until the window is closed
    {
        // Calculate time delta
        frameDelta = std::chrono::duration<float, std::milli>(t2 - t1).count(); // The time taken for a cycle to run
        t1 = std::chrono::high_resolution_clock::now(); // Get first time point

        // Get input

        glfwPollEvents(); // Tell GLFW to poll input events
        if (glfwGetKey(window, GLFW_KEY_A)) // Move left if A is pressed
        {
            camera.velocity -= glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * frameDelta;
        }
        if (glfwGetKey(window, GLFW_KEY_D)) // Move right if D is pressed
        {
            camera.velocity += glm::normalize(glm::cross(camera.front, camera.up)) * camera.speed * frameDelta;
        }
        if (glfwGetKey(window, GLFW_KEY_S)) // Move backwards if S is pressed
        {
            camera.velocity -= camera.speed * glm::vec3(cos(glm::radians(camera.yaw)), 0.0f, sin(glm::radians(camera.yaw))) * glm::vec3(1.0f, 0.0f, 1.0f) * frameDelta;
        }
        if (glfwGetKey(window, GLFW_KEY_W)) // Move forwards if W is pressed
        {
            camera.velocity += camera.speed * glm::vec3(cos(glm::radians(camera.yaw)), 0.0f, sin(glm::radians(camera.yaw))) * glm::vec3(1.0f, 0.0f, 1.0f) * frameDelta;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) // Move down if left shift is pressed
        {
            camera.velocity -= camera.speed * camera.up * frameDelta;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE)) // Move up if space is pressed
        {
            camera.velocity += camera.speed * camera.up * frameDelta;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) // Terminate program if escape is pressed
        {
            break; // Break out of main program loop
        }
        if (glfwGetKey(window, GLFW_KEY_E))
        {
            addNewCube(camera.xyz.x, camera.xyz.y, camera.xyz.z);
        }

        // Set camera position and velocity
        camera.xyz += camera.velocity * camera.speed * frameDelta;
        camera.velocity -= camera.velocity * camera.smoothing * frameDelta;

        View = glm::lookAt(camera.xyz, camera.xyz + camera.front, camera.up); // Set view matrix

        // Send matrices to shader
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(Model)); // Model matrix
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(View)); // View matrix
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(Projection)); // Projection matrix

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        glDrawElements(GL_TRIANGLES, cubeCount * 36, GL_UNSIGNED_INT, nullptr); // Draw elements (the amount of integers in the element array)

        glfwSwapBuffers(window); // Swap buffers

        t2 = std::chrono::high_resolution_clock::now(); // Get second time point
    }
    // Terminate program if window is closed or loop is exited
    glfwTerminate(); // Terminate GLFW
    return 0; // Return 0 after terminating GLFW
}