/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[burningship.frag]
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

vec3 palette(float t) {
    return u_palette_a + u_palette_b * cos(6.28318 * (u_palette_c * t + u_palette_d));
}

void main()
{
    dvec2 uv = (dvec2(TexCoords) * 2.0 - 1.0);
    uv.x *= double(iResolution.x) / double(iResolution.y);
    dvec2 c = uv / u_zoom + u_center;
    dvec2 z = dvec2(0.0);
    int i;
    for (i = 0; i < u_max_iterations; i++)
    {
        z = abs(z);
        double x_temp = z.x * z.x - z.y * z.y + c.x;
        z.y = 2.0 * z.x * z.y + c.y;
        z.x = x_temp;
        if (dot(z, z) > 4.0)
        {
            break;
        }
    }

    if (i == u_max_iterations)
    {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        float magnitude = float(dot(z, z));
        float smooth_i = float(i) - log2(log2(magnitude));
        float color_val = smooth_i / float(u_max_iterations);
        vec3 color = palette(color_val);
        FragColor = vec4(color, 1.0);
    }
}