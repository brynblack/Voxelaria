R"(

#version 330 core

in vec4 vs_color;
out vec4 fs_color;

void main()
{
    fs_color = vs_color;
};

)"
