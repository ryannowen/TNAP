#version 330

uniform mat4 combined_xform;
uniform mat4 model_xform;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv;

//out vec4 varying_colour;
out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_uv;

void main(void)
{	
	//gl_Position = model_xform * combined_xform * vec4(vertex_position, 1.0);
	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
	varying_position = vec3(model_xform * vec4(vertex_position, 1));
	//varying_position = vertex_position;
	varying_normal = normalize(mat3(model_xform) * vertex_normal);
	varying_uv = vertex_uv;


	//varying_colour = normalize(mat3(model_xform) * vertex_normal);
}