#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv;
layout(location = 5) in mat4 aInstanceMatrix;

uniform mat4 combined_xform;

out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_uv;

void main(void)
{	
	varying_position = vec3(aInstanceMatrix * vec4(vertex_position, 1));
	varying_normal = normalize(mat3(aInstanceMatrix) * vertex_normal);
	varying_uv = vertex_uv;

	gl_Position = combined_xform * aInstanceMatrix * vec4(vertex_position, 1.0);
}