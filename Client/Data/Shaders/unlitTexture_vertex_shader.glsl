#version 330

uniform mat4 combined_xform;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv;
layout(location = 3) in mat4 aInstanceMatrix;

out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_uv;

void main(void)
{	
	varying_position = vertex_position;
	varying_normal = vertex_normal;
	varying_uv = vertex_uv;

	gl_Position = combined_xform * aInstanceMatrix * vec4(vertex_position, 1.0);
}