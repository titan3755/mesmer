/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[texture_view.frag]
*/

#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform dvec2 u_center;
uniform double u_zoom;

void main()
{
    dvec2 uv = (TexCoords - 0.5) / u_zoom + 0.5 + u_center;
    FragColor = texture(screenTexture, vec2(uv));
}