#version 330

//struct SMaterial
//{
//	vec4 m_colourTint;
//
//	sampler2D m_texture;
//	sampler2D m_normalTexture;
//	sampler2D m_metallicTexture;
//	sampler2D m_roughnessTexture;
//	sampler2D m_AOTexture;
//
//	sampler2D m_emissionTexture;
//	vec3 m_emissionColour;
//	float m_emissionIntensity;
//};

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitTangent;
layout(location = 5) in mat4 aInstanceMatrix;

//uniform SMaterial material;

uniform vec3 camera_position;

uniform mat4 combined_xform;

out vec3 varying_position;
//out vec3 varying_normal;
out vec2 varying_uv;

out mat3 varying_TBN;

out vec3 tangentViewPos;
out vec3 tangentFragPos;

void main(void)
{	
	varying_position = vec3(aInstanceMatrix * vec4(vertex_position, 1));
	varying_uv = vertex_uv;
	//varying_normal = vertex_normal;
	//varying_normal = normalize(mat3(aInstanceMatrix) * vertex_normal);

	mat3 normalMatrix = transpose(inverse(mat3(aInstanceMatrix)));
	vec3 T = normalize(normalMatrix * vertex_tangent);
	vec3 N = normalize(normalMatrix * vertex_normal);

	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	varying_TBN = transpose(mat3(T, B, N));


	tangentViewPos = varying_TBN * camera_position;
	tangentFragPos = varying_TBN * varying_position;

	gl_Position = combined_xform * aInstanceMatrix * vec4(vertex_position, 1.0);
}