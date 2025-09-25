/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[newton.frag]
*/

#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
uniform dvec2 u_center;
uniform double u_zoom;
uniform int u_max_iterations;
uniform vec2 iResolution;
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

void main() {
    dvec2 uv = (dvec2(TexCoords) * 2.0 - 1.0);
    uv.x *= double(iResolution.x) / double(iResolution.y);
    dvec2 z = uv / u_zoom + u_center;

    dvec2 root1 = dvec2(1.0, 0.0);
    dvec2 root2 = dvec2(-0.5, 0.866025);
    dvec2 root3 = dvec2(-0.5, -0.866025);
    double tolerance = 0.0001;

    int i;
    for (i = 0; i < u_max_iterations; i++) {
        dvec2 z2 = cmult(z, z);
        dvec2 z3 = cmult(z2, z);
        z = cdiv(dvec2(2.0, 0.0) * z3 + dvec2(1.0, 0.0), dvec2(3.0, 0.0) * z2);

        if (distance(z, root1) < tolerance || distance(z, root2) < tolerance || distance(z, root3) < tolerance) {
            break;
        }
    }

    float brightness = 1.0 - float(i) / float(u_max_iterations);
    vec3 color = vec3(0.0);
    if (distance(z, root1) < tolerance) color = vec3(1.0, 0.2, 0.2);
    else if (distance(z, root2) < tolerance) color = vec3(0.2, 1.0, 0.2);
    else if (distance(z, root3) < tolerance) color = vec3(0.2, 0.2, 1.0);

    FragColor = vec4(color * brightness, 1.0);
}