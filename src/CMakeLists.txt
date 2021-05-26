set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
add_compile_options("-DGLEW_STATIC")
set(CMAKE_CXX_FLAGS "-static")

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable("cubicspeed" "cubicspeed.cpp")
target_link_libraries("cubicspeed" glew32;glfw3;opengl32;user32;gdi32;shell32)
