#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;

out vec3 colour;

void main(void)
{	
	//colour = vertex_colour;
	gl_Position.xyz = vertex_position;
	gl_Position.w = 1.0;
}