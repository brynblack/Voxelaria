#include "main.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <format>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <random>
#include <string>

#include "logger.hpp"

const GLfloat gravity = 0.000035;
const int platformSize = 50;

GLuint vbo, ibo, shader;

Voxelaria::camera camera;
Voxelaria::mouse mouse;

GLint cubeCount = 0;
GLfloat frameTime;
glm::vec3 positions[1000000];
glm::mat4 Projection;

using namespace logger;

static unsigned int CompileShader(unsigned int type,
                                  const std::string& source) {
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char*)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    error(std::format("Failed to compile {} shader",
                      (type == GL_VERTEX_SHADER ? "vertex" : "fragment")));
    error(message);
    glDeleteShader(id);
    return 0;
  }
  return id;
}

static unsigned int CreateShader(const std::string& vertexShader,
                                 const std::string& fragmentShader) {
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

bool checkBlockCollision(glm::vec3 xyz) {
  for (int i = 0; i < cubeCount; ++i) {
    if (positions[i] == glm::floor(xyz)) {
      return true;
    }
  }
  return false;
}

bool checkCameraCollision(glm::vec3 xyz) {
  for (int i = 0; i < cubeCount; ++i) {
    glm::vec3 pos = positions[i];
    xyz = glm::floor(xyz);
    if (pos == xyz || pos == glm::vec3(xyz.x, xyz.y + 1, xyz.z) ||
        pos == glm::vec3(xyz.x, xyz.y - 1, xyz.z)) {
      return true;
    }
  }
  return false;
}

void createNewVoxel(glm::vec3 xyz) {
  if (!checkBlockCollision(xyz)) {
    ++cubeCount;
    int offset = cubeCount - 1;
    xyz = glm::floor(xyz);
    positions[offset] = xyz;

    GLfloat vertices[48] = {
        xyz.x + 1.0f, xyz.y + 1.0f, xyz.z + 0.0f, 1.0f, 0.0f, 0.0f,
        xyz.x + 1.0f, xyz.y + 0.0f, xyz.z + 0.0f, 0.0f, 1.0f, 0.0f,
        xyz.x + 0.0f, xyz.y + 0.0f, xyz.z + 0.0f, 0.0f, 0.0f, 1.0f,
        xyz.x + 0.0f, xyz.y + 1.0f, xyz.z + 0.0f, 0.0f, 0.0f, 0.0f,
        xyz.x + 1.0f, xyz.y + 1.0f, xyz.z + 1.0f, 0.0f, 1.0f, 0.0f,
        xyz.x + 0.0f, xyz.y + 1.0f, xyz.z + 1.0f, 0.0f, 0.0f, 1.0f,
        xyz.x + 1.0f, xyz.y + 0.0f, xyz.z + 1.0f, 0.0f, 0.0f, 0.0f,
        xyz.x + 0.0f, xyz.y + 0.0f, xyz.z + 1.0f, 0.0f, 0.0f, 1.0f};
    GLint indices[36] = {
        offset * 8 + 0, offset * 8 + 1, offset * 8 + 2, offset * 8 + 2,
        offset * 8 + 3, offset * 8 + 0, offset * 8 + 4, offset * 8 + 0,
        offset * 8 + 3, offset * 8 + 3, offset * 8 + 5, offset * 8 + 4,
        offset * 8 + 4, offset * 8 + 6, offset * 8 + 1, offset * 8 + 1,
        offset * 8 + 0, offset * 8 + 4, offset * 8 + 5, offset * 8 + 7,
        offset * 8 + 6, offset * 8 + 6, offset * 8 + 4, offset * 8 + 5,
        offset * 8 + 3, offset * 8 + 2, offset * 8 + 7, offset * 8 + 7,
        offset * 8 + 5, offset * 8 + 3, offset * 8 + 1, offset * 8 + 6,
        offset * 8 + 7, offset * 8 + 7, offset * 8 + 2, offset * 8 + 1};

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, offset * 48 * (GLintptr)sizeof(GLfloat),
                    48 * sizeof(GLfloat), vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    offset * 36 * (GLintptr)sizeof(GLint), 36 * sizeof(GLint),
                    indices);
  }
}

void deleteVoxel(glm::vec3 xyz) {
  for (int i = 0; i < cubeCount; ++i) {
    if (positions[i] == glm::floor(xyz)) {
      size_t size = sizeof positions / sizeof positions[0];
      for (size_t ind = i; ind <= size - 1; ++ind) {
        positions[i] = positions[ind + 1];
      }
      positions[size - 1] = {};
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glClearBufferSubData(GL_ARRAY_BUFFER, GL_R32F,
                           i * 48 * (GLintptr)sizeof(GLfloat),
                           48 * sizeof(GLfloat), GL_RED, GL_FLOAT, nullptr);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glClearBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_R32F,
                           i * 36 * (GLintptr)sizeof(GLfloat),
                           36 * sizeof(GLfloat), GL_RED, GL_INT, nullptr);
    }
  }
}

void generate_world() {
  for (int x = -platformSize; x < platformSize; ++x) {
    for (int z = -platformSize; z < platformSize; ++z) {
      createNewVoxel(glm::vec3(x, 0, z));
    }
  }
}

void mouse_callback(__attribute__((unused)) GLFWwindow* window, double x,
                    double y) {
  if (mouse.firstMouse) {
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

  if (camera.pitch > 89.0f) {
    camera.pitch = 89.0f;
  }
  if (camera.pitch < -89.0f) {
    camera.pitch = -89.0f;
  }

  camera.direction.x =
      (float)(cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)));
  camera.direction.y = (float)(sin(glm::radians(camera.pitch)));
  camera.direction.z =
      (float)(sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch)));
  camera.front = glm::normalize(camera.direction);
}

void mouse_button_callback(__attribute__((unused)) GLFWwindow* window,
                           int button, int action,
                           __attribute__((unused)) int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    deleteVoxel(glm::vec3(camera.xyz.x, camera.xyz.y - 2, camera.xyz.z));
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    createNewVoxel(glm::vec3(camera.xyz.x, camera.xyz.y - 2, camera.xyz.z));
  }
}

void key_callback(__attribute__((unused)) GLFWwindow* window, int key,
                  __attribute__((unused)) int scancode, int action,
                  __attribute__((unused)) int mods) {
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && !camera.flying &&
      !checkCameraCollision(
          camera.xyz + glm::vec3(0.0f, camera.velocity.y, 0.0f) * frameTime) &&
      checkCameraCollision(
          glm::vec3(camera.xyz.x, camera.xyz.y - 0.1, camera.xyz.z) +
          glm::vec3(0.0f, camera.velocity.y, 0.0f) * frameTime)) {
    camera.velocity += camera.jumpHeight * camera.up;
  }
}

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow* window,
                               int width, int height) {
  Projection =
      glm::perspective(glm::radians(camera.fov),
                       (GLfloat)width / (GLfloat)height, 0.01f, 1000.0f);
  glViewport(0, 0, width, height);
}

int main() {
  if (!glfwInit()) {
    error("Unable to initialise GLFW");
    return -1;
  }

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  auto* mode = (GLFWvidmode*)glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Voxelaria",
                                        monitor, nullptr);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!window) {
    error("Unable to create GLFW window");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  if (glewInit() != GLEW_OK) {
    error("Unable to initialise GLEW");
    glfwTerminate();
    return -1;
  }

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, 10000000, nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (GLvoid*)nullptr);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (GLvoid*)(3 * sizeof(float)));

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 10000000, nullptr, GL_DYNAMIC_DRAW);

  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // clang-format off
  std::string vertexShader = {
    #include "shader.vert"
  };

  std::string fragmentShader = {
    #include "shader.frag"
  };
  // clang-format on

  shader = CreateShader(vertexShader, fragmentShader);
  glUseProgram(shader);

  camera.set_xyz(glm::vec3(0.0f, 2.0f, 0.0f));
  camera.set_front(glm::vec3(0.0f, 0.0f, -1.0f));
  camera.set_up(glm::vec3(0.0f, 1.0f, 0.0f));
  camera.set_pitch(0.0);
  camera.set_yaw(-90.0);
  camera.set_fov(90.0f);
  camera.set_speed(1.0f * 0.00005);
  camera.set_smoothing(1.0f * 0.01);
  camera.set_jumpHeight(1.0f * 0.01);
  camera.enable_flying(false);

  mouse.lastX = mode->width / 2.0;
  mouse.lastY = mode->height / 2.0;
  mouse.sensitivity = 0.1f;
  mouse.firstMouse = true;

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glm::mat4 Model = glm::mat4(1.0f);
  glm::mat4 View = glm::mat4(1.0f);

  GLint model_location = glGetUniformLocation(shader, "model");
  GLint view_location = glGetUniformLocation(shader, "view");
  GLint projection_location = glGetUniformLocation(shader, "projection");

  std::chrono::high_resolution_clock::time_point t1, t2;

  generate_world();

  while (!glfwWindowShouldClose(window)) {
    frameTime = std::chrono::duration<float, std::milli>(t2 - t1).count();
    t1 = std::chrono::high_resolution_clock::now();

    glfwPollEvents();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      break;
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
      camera.velocity -= glm::normalize(glm::cross(camera.front, camera.up)) *
                         camera.speed * frameTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
      camera.velocity += glm::normalize(glm::cross(camera.front, camera.up)) *
                         camera.speed * frameTime;
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
      camera.velocity -= glm::vec3(cos(glm::radians(camera.yaw)), 0.0f,
                                   sin(glm::radians(camera.yaw))) *
                         glm::vec3(1.0f, 0.0f, 1.0f) * camera.speed * frameTime;
    }
    if (glfwGetKey(window, GLFW_KEY_W)) {
      camera.velocity += glm::vec3(cos(glm::radians(camera.yaw)), 0.0f,
                                   sin(glm::radians(camera.yaw))) *
                         glm::vec3(1.0f, 0.0f, 1.0f) * camera.speed * frameTime;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) && camera.flying) {
      camera.velocity += glm::vec3(0.0f, 1.0f, 0.0f) * camera.speed * frameTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) && camera.flying) {
      camera.velocity -= glm::vec3(0.0f, 1.0f, 0.0f) * camera.speed * frameTime;
    }

    if (!camera.flying) {
      camera.velocity -= glm::vec3(
          camera.velocity.x * camera.smoothing * frameTime, gravity * frameTime,
          camera.velocity.z * camera.smoothing * frameTime);
    } else {
      camera.velocity -= camera.velocity * camera.smoothing * frameTime;
    }

    if (checkCameraCollision(camera.xyz +
                             glm::vec3(camera.velocity.x, 0.0f, 0.0f) *
                                 frameTime)) {
      camera.velocity *= glm::vec3(0.0f, 1.0f, 1.0f);
    }
    if (checkCameraCollision(camera.xyz +
                             glm::vec3(0.0f, camera.velocity.y, 0.0f) *
                                 frameTime)) {
      camera.velocity *= glm::vec3(1.0f, 0.0f, 1.0f);
    }
    if (checkCameraCollision(camera.xyz +
                             glm::vec3(0.0f, 0.0f, camera.velocity.z) *
                                 frameTime)) {
      camera.velocity *= glm::vec3(1.0f, 1.0f, 0.0f);
    }
    if (checkCameraCollision(camera.xyz + camera.velocity * frameTime)) {
      camera.velocity.y += 0.01f;
    }

    camera.xyz += camera.velocity * frameTime;

    View = glm::lookAt(camera.xyz, camera.xyz + camera.front, camera.up);

    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(Model));
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(View));
    glUniformMatrix4fv(projection_location, 1, GL_FALSE,
                       glm::value_ptr(Projection));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, (GLsizei)cubeCount * 36, GL_UNSIGNED_INT,
                   nullptr);
    glfwSwapBuffers(window);

    t2 = std::chrono::high_resolution_clock::now();
  }
  glfwTerminate();
  return 0;
}
