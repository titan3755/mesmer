#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec2 iResolution;
uniform float iTime;

// Wavy infinity symbol effect
void main() {
    vec2 uv = (TexCoords * 2.0 - 1.0);
    uv.x *= iResolution.x / iResolution.y;

    float t = iTime * 0.5;
    
    // Infinity symbol shape (lemniscate of Bernoulli)
    float a = uv.x * uv.x - uv.y * uv.y;
    float b = 2.0 * uv.x * uv.y;
    float infinity = (a*a + b*b) - 0.2 * (uv.x*uv.x + uv.y*uv.y);
    
    // Make it wavy
    infinity += 0.02 * sin(uv.x * 20.0 + t * 2.0) * cos(uv.y * 15.0 - t * 2.0);
    
    // Calculate distance and draw
    float dist = abs(infinity) * 15.0;
    float glow = 0.01 / dist;
    
    vec3 col = glow * vec3(0.1, 0.5, 1.0); // Blue glow
    
    FragColor = vec4(col, 1.0);
}