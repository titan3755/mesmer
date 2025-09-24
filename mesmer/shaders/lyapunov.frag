/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[lyapunov.frag]
*/

#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec2 iResolution;
uniform dvec2 u_lyapunov_center;
uniform double u_lyapunov_zoom;
uniform int u_max_iterations;

uniform vec3 u_palette_a;
uniform vec3 u_palette_b;
uniform vec3 u_palette_c;
uniform vec3 u_palette_d;
uniform float u_color_density;

const int sequence[4] = int[4](0, 1, 0, 1);
const int sequence_len = 4;

vec3 palette(float t) {
    return u_palette_a + u_palette_b * cos(6.28318 * (u_palette_c * t + u_palette_d));
}

void main()
{
    dvec2 uv = (dvec2(TexCoords) * 2.0 - 1.0);
    dvec2 ab = uv / u_lyapunov_zoom + u_lyapunov_center;

    dvec2 r_params = dvec2(ab.x, ab.y);
    double x = 0.5;
    double lambda = 0.0;
    int n_warmup = 100;
    int n_sum = 400;

    for (int i = 0; i < n_warmup + n_sum; i++) {
        int r_index = sequence[i % sequence_len];
        double r = (r_index == 0) ? r_params.x : r_params.y;
        
        x = r * x * (1.0 - x);
        
        if (i >= n_warmup) {
            lambda += log(float(abs(r * (1.0 - 2.0 * x))));
        }
    }

    lambda /= double(n_sum);

    if (lambda > 0.0) { 
        FragColor = vec4(palette(float(lambda) * 2.5f), 1.0);
    } else { 
        float stability = clamp(abs(float(lambda)) * 2.0, 0.0, 1.0);
        FragColor = vec4(vec3(0.0, 0.1, 0.5) * stability, 1.0);
    }
}