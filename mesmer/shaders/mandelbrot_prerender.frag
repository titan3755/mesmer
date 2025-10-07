/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[mandelbrot_prerender.frag]
*/

#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform dvec2 u_center;
uniform double u_zoom;
uniform int u_max_iterations;
uniform vec2 iResolution;
uniform vec3 u_palette_a;
uniform vec3 u_palette_b;
uniform vec3 u_palette_c;
uniform vec3 u_palette_d;
uniform float u_color_density;
uniform ivec4 u_tile_info;

vec3 palette(float t) {
    return u_palette_a + u_palette_b * cos(6.28318 * (u_palette_c * t + u_palette_d));
}

