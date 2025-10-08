#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
uniform vec2 iResolution;
uniform float iTime;

vec3 palette(float t) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);
    return a + b * cos(6.28318 * (c * t + d));
}

void main() {
    vec2 uv = TexCoords * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    float t = iTime * 0.6;
    float a = uv.x * uv.x - uv.y * uv.y;
    float b = 2.0 * uv.x * uv.y;
    float infinity = (a*a + b*b) - 0.2 * (uv.x*uv.x + uv.y*uv.y);
    infinity += 0.03 * sin(uv.x * 15.0 + t * 3.0) * cos(uv.y * 15.0 - t * 2.0);
    infinity += 0.015 * sin(length(uv) * 10.0 - t * 4.0);
    float dist = abs(infinity) * 15.0;
    float glow = 0.015 / (dist + 0.1);
    float shimmer = 0.5 + 0.5 * sin((uv.x + uv.y) * 25.0 + t * 5.0);
    glow *= mix(0.8, 1.2, shimmer);
    float hue = atan(uv.y, uv.x) / 6.28318 + 0.5 + t * 0.1;
    vec3 col = palette(hue) * glow * 1.4;
    FragColor = vec4(col, 1.0);
}
