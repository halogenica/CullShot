#version 120
// input
uniform vec4 LightPosition; // Light position in eye coords
uniform vec3 Ld;            // Light intensity

// output
varying vec3 LightIntensity;

void main()
{
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec4 eyeCoords = gl_ModelViewMatrix * vec4(gl_Position);

    // If light is relative to the camera
    vec3 s = normalize(vec3(LightPosition - eyeCoords));
    // If light is relative to world space NOT WORKING
//    vec3 s = normalize((LightPosition * gl_ModelViewProjectionMatrix).xyz);

    LightIntensity = Ld * gl_Color.rgb * max(dot(s, normal), 0.0);

    gl_Position = ftransform();
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
