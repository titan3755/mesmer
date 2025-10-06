#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
uniform mat4 u_tile_transform;

void main()
{
    gl_Position = u_tile_transform * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}