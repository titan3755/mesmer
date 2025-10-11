/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[burning_ship_prerender.frag]
*/

#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
uniform dvec2 u_center;
uniform double u_zoom;
uniform int u_max_iterations;
uniform vec3 u_palette_a, u_palette_b, u_palette_c, u_palette_d;
uniform float u_color_density;
uniform ivec4 u_tile_info;
vec3 palette(float t) {
    return u_palette_a + u_palette_b * cos(6.28318 * (u_palette_c * t + u_palette_d));
}
void main()
{
    dvec2 global_uv = (dvec2(u_tile_info.x, u_tile_info.y) + TexCoords) / dvec2(u_tile_info.z, u_tile_info.w);
    global_uv = global_uv * 2.0 - 1.0;
    dvec2 c = global_uv / u_zoom + u_center;
    dvec2 z = dvec2(0.0);
    int i;
    for (i = 0; i < u_max_iterations; i++) {
        z.x = abs(z.x);
        z.y = abs(z.y);
        z = dvec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
        if (dot(z, z) > 4.0) break;
    }
    if (i == u_max_iterations) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else {
        float magnitude = float(dot(z, z));
        float smooth_i = float(i) - log2(log2(magnitude));
        float color_val = smooth_i * u_color_density;
        FragColor = vec4(palette(color_val), 1.0);
    }
}