#version 330 core
layout (location = 0) out vec4 res;

in vert_input
{
    vec2 position;
    vec4 color;
} frag;

void main()
{
    res = frag.color;
}