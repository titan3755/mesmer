/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[phoenix.frag]
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

uniform dvec2 u_phoenix_c; 
uniform double u_phoenix_p;

vec3 palette(float t) 
{
    return u_palette_a + u_palette_b * cos(6.28318 * (u_palette_c * t + u_palette_d));
}

void main()
{
    dvec2 uv = (dvec2(TexCoords) * 2.0 - 1.0);
    uv.x *= double(iResolution.x) / double(iResolution.y);
    
    dvec2 z = uv / u_zoom + u_center;
    dvec2 z_prev = dvec2(0.0);

    int i;
    for (i = 0; i < u_max_iterations; i++)
    {
        dvec2 z_temp = z;
        
        z.x = z.x * z.x - z.y * z.y + u_phoenix_c.x + u_phoenix_p * z_prev.x;
        z.y = 2.0 * z_temp.x * z_temp.y + u_phoenix_c.y + u_phoenix_p * z_prev.y;

        z_prev = z_temp;

        if (dot(z, z) > 4.0) break;
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