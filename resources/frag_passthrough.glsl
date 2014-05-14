#version 120

uniform sampler2D   tex0;
uniform vec2        resolution;
uniform float       resolutionScale;
uniform float       timer;

#define _PI 3.1415926535897932384626433832795

void main(void)
{
    gl_FragColor = texture2D(tex0, gl_FragCoord.xy / resolution.xy);
}
