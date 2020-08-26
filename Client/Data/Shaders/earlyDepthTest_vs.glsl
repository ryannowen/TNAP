#version 430

layout(location = 0) in vec3 vertex_position;
layout(location = 5) in mat4 aInstanceMatrix;

uniform mat4 combined_xform;

void main(void)
{	
	gl_Position = combined_xform * aInstanceMatrix * vec4(vertex_position, 1.0);
}