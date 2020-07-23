#version 330

out vec4 color;

in vec3 colour;
in vec2 varying_uv;

uniform sampler2D texSample;
uniform vec3 colourUni;


void main(void)
{
   // color = texture(texSample, varying_uv);
   // color = vec4(colour, 1.0);
   // color = vec4(1.0, 0.0, 0.0, 1.0);
    color = vec4(colourUni, 1.0);
}