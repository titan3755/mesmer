/*
	Mesmer - An interactive and high-performance fractal generator and explorer.
	Made by -> Titan // GH: https://github.com/titan3755/mesmer
	[background.frag]
*/

#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
uniform float iTime;
uniform vec2 iResolution;
void main() {
    vec2 uv = (TexCoords * 2.0 - 1.0) * vec2(iResolution.x / iResolution.y, 1.0);
    float wave = sin(uv.x * 2.0 + iTime * 2.2) * cos(uv.y * 5.2 - iTime);
    vec3 color1 = vec3(0.0, 0.1, 0.4);
    vec3 color2 = vec3(0.1, 0.0, 0.3);
    vec3 accent = vec3(1.0, 0.2, 0.5);
    vec3 base = mix(color1, color2, 0.5 + 0.5 * wave);
    float accentFactor = 0.4 * (0.5 + 0.5 * sin(iTime * 3.0 + uv.x * 4.0 + uv.y * 4.0));
    vec3 finalColor = base + accent * accentFactor;
    FragColor = vec4(finalColor, 1.0);
}