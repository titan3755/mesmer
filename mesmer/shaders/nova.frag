/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[nova.frag]
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
uniform double u_power;
uniform double u_relaxation;

dvec2 cmult(dvec2 a, dvec2 b) { return dvec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x); }
dvec2 cdiv(dvec2 a, dvec2 b) {
    double denom = b.x*b.x + b.y*b.y;
    return dvec2((a.x*b.x + a.y*b.y)/denom, (a.y*b.x - a.x*b.y)/denom);
}
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
    dvec2 uv = (dvec2(TexCoords) * 2.0 - 1.0);
    uv.x *= double(iResolution.x) / double(iResolution.y);
    dvec2 c = uv / u_zoom + u_center;
    dvec2 z = dvec2(1.0, 0.0);
    
    int i;
    for (i = 0; i < u_max_iterations; i++) {
        dvec2 z_p = cpow(z, u_power);
        dvec2 z_p_minus_1 = cpow(z, u_power - 1.0);
        dvec2 term = cdiv(z_p - dvec2(1.0, 0.0), dvec2(u_power, 0.0) * z_p_minus_1);
        z = z - u_relaxation * term + c;
        if (dot(z, z) > 100.0) break;
    }

    if (i == u_max_iterations)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        float magnitude = float(dot(z, z));
        float smooth_i = float(i) - log2(log2(magnitude));
        float color_val = smooth_i * u_color_density;
        vec3 color = palette(color_val);
        FragColor = vec4(color, 1.0);
    }
}