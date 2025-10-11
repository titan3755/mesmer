/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[multibrot_prerender.frag]
*/

#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
uniform dvec2 u_center;
uniform double u_zoom;
uniform int u_max_iterations;
uniform double u_power;
uniform vec3 u_palette_a, u_palette_b, u_palette_c, u_palette_d;
uniform float u_color_density;
uniform ivec4 u_tile_info;
dvec2 cpow(dvec2 z, double exponent) {
    float z_x = float(z.x);
    float z_y = float(z.y);
    float exp = float(exponent);
    float r = sqrt(z_x*z_x + z_y*z_y);
    float theta = atan(z_y, z_x);
    float r_pow = pow(r, exp);
    float new_x = r_pow * cos(exp * theta);
    float new_y = r_pow * sin(exp * theta);
    return dvec2(new_x, new_y);
}
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
        z = cpow(z, u_power) + c;
        if (dot(z, z) > 4.0) break;
    }
    if (i == u_max_iterations) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else {
        float magnitude = float(dot(z, z));
        float smooth_i = float(i) - log2(log2(magnitude));
        float color_val = smooth_i * u_color_density;
        vec3 color = palette(color_val);
        FragColor = vec4(color, 1.0);
    }
}