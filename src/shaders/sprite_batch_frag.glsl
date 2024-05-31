#version 330 core
layout (location = 0) out vec4 res;
#define NUM_TEXTURES 32

in vert_input
{
    vec2 position;
    vec2 texcoord;
    vec4 color;
    flat uint texture;
} frag;

uniform sampler2D buffers[NUM_TEXTURES];

void main()
{
    res = frag.color * texture(buffers[frag.texture], frag.texcoord);
}