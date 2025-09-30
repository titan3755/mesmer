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
uniform vec2 iResolution;

void main()
{
    float screenAspect = iResolution.x / iResolution.y;
    dvec2 uv = TexCoords;
    uv -= 0.5;
    if (screenAspect > 1.0) {
        uv.x *= screenAspect;
    } else {
        uv.y /= screenAspect;
    }
    uv = uv / u_zoom + u_center;
    uv += 0.5;
    FragColor = texture(screenTexture, vec2(uv));
}