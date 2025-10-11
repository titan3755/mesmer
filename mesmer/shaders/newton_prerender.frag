/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[newton_prerender.frag]
*/

#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
uniform dvec2 u_center;
uniform double u_zoom;
uniform int u_max_iterations;
uniform ivec4 u_tile_info;
uniform vec3 u_palette_a, u_palette_b, u_palette_c, u_palette_d;
uniform float u_color_density;
dvec2 cmult(dvec2 a, dvec2 b) { return dvec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x); }
dvec2 cdiv(dvec2 a, dvec2 b) {
    double denom = b.x*b.x + b.y*b.y;
    return dvec2((a.x*b.x + a.y*b.y)/denom, (a.y*b.x - a.x*b.y)/denom);
}
vec3 palette(float t) 
{
    return u_palette_a + u_palette_b * cos(6.28318 * (u_palette_c * t + u_palette_d));
}
void main()
{
    dvec2 global_uv = (dvec2(u_tile_info.x, u_tile_info.y) + TexCoords) / dvec2(u_tile_info.z, u_tile_info.w);
    global_uv = global_uv * 2.0 - 1.0;
    dvec2 z = global_uv / u_zoom + u_center;
    dvec2 root1 = dvec2(1.0, 0.0);
    dvec2 root2 = dvec2(-0.5, 0.866025);
    dvec2 root3 = dvec2(-0.5, -0.866025);
    double tolerance = 0.0001;
    int i;
    for (i = 0; i < u_max_iterations; i++) {
        dvec2 z2 = cmult(z, z);
        dvec2 z3 = cmult(z2, z);
        if (dot(z2, z2) < 1e-12) break;
        z = cdiv(dvec2(2.0, 0.0) * z3 + dvec2(1.0, 0.0), dvec2(3.0, 0.0) * z2);
        if (distance(z, root1) < tolerance || distance(z, root2) < tolerance || distance(z, root3) < tolerance) break;
    }
    float base_hue = 0.0;
    if (distance(z, root1) < tolerance)      base_hue = u_palette_a.x;
    else if (distance(z, root2) < tolerance) base_hue = u_palette_b.y;
    else if (distance(z, root3) < tolerance) base_hue = u_palette_c.z;
    vec3 color = vec3(0.0);
    if (base_hue > 0.0) {
        float color_val = base_hue - float(i) * u_color_density * 2.0;
        color = palette(color_val);
    }
    FragColor = vec4(color, 1.0);
}