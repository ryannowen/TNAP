#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
layout(location = 2) in vec2 vertex_uv;

out vec3 colour;
out vec2 varying_uv;

void main(void)
{	
	varying_uv = vertex_uv;
	colour = vertex_colour;
	//gl_Position = vec4(vertex_position, 1.0);
	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
}