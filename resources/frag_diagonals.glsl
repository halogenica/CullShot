#version 120

uniform vec2        resolution;
uniform float       resolutionScale;
uniform float       timer;

#define _PI 3.1415926535897932384626433832795

void main(void)
{
	gl_FragColor = gl_Color;

	// This produces a diagonal line:
	// 		sin ( ( gl_FragCoord.x - gl_FragCoord.y ) / resolution.x )
	// The coordinate computation is multiplied by a period to control the number of lines
	// The timer is added (and multiplied by a rate) to animate the lines
	// The floor function rounds the results up or down to make solid black/white lines

    gl_FragColor.rgb *= floor( sin( ( ( gl_FragCoord.x - gl_FragCoord.y ) / resolution.x*80 / resolutionScale ) + timer*10 ) + 0.5);
}
