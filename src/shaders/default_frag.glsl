#version 330 core
layout (location = 0) out vec4 res;

in vert_input
{
    vec2 position;
    vec2 texcoord;
} frag;

uniform sampler2D window;

void main()
{
    res = texture(window, frag.texcoord);
}