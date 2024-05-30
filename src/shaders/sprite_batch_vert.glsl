#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec4 color;
layout (location = 3) in uint texture;

out vert_input
{
    vec2 position;
    vec2 texcoord;
    vec4 color;
    flat uint texture;
} vert;

void main()
{
    vert.texcoord = texcoord;
    vert.color = color;
    vert.texture = texture;
    gl_Position = vec4(position, 0.0, 1.0);
}