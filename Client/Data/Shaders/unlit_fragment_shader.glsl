#version 330

//in vec3 varying_position;
//in vec2 varying_uv;
//in vec3 varying_normal;

out vec4 fragment_colour;


void main(void)
{
	//fragment_colour = vec4(varying_normal, 1.0);
	fragment_colour = vec4(0, 1, 0, 1);
}