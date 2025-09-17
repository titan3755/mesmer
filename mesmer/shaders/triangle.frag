/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[triangle.frag]
*/

#version 460 core
out vec4 FragColor;

in vec3 ourColor;

void main() {
    FragColor = vec4(ourColor, 1.0f);
}
