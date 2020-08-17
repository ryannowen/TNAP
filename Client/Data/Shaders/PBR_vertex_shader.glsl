#version 330
//
//struct SPointLightData
//{
//	vec3 m_position;
//	vec3 m_colour;
//	float m_intensity;
//
//	float m_range;
//};
//
//struct SSpotLightData
//{
//	vec3 m_position;
//	vec3 m_direction;
//	vec3 m_colour;
//	float m_intensity;
//
//	float m_range;
//
//	float m_fov;
//};

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_uv;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitTangent;
layout(location = 5) in mat4 aInstanceMatrix;

uniform vec3 camera_position;

uniform mat4 combined_xform;

//#define POINT_LIGHT_AMOUNT uint(50)
//#define SPOT_LIGHT_AMOUNT uint(50)
//
//uniform SPointLightData pointLightData[POINT_LIGHT_AMOUNT];
//uniform uint amountOfPointLightData;
//
//uniform SSpotLightData spotLightData[SPOT_LIGHT_AMOUNT];
//uniform uint amountOfSpotLightData;

out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_uv;

out vec3 tangentViewPos;
out vec3 tangentFragPos;

out mat3 varying_TBN;

void main(void)
{	
	varying_position = vec3(aInstanceMatrix * vec4(vertex_position, 1));
	varying_uv = vertex_uv;
	varying_normal = normalize(mat3(aInstanceMatrix) * vertex_normal);

	mat3 normalMatrix = transpose(inverse(mat3(aInstanceMatrix)));
	vec3 T = normalize(normalMatrix * vertex_tangent);
	vec3 N = normalize(normalMatrix * varying_normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	varying_TBN = TBN;

	/*for (uint i = uint(0); i < amountOfPointLightData; i++)
	{
		vec3 lightPos = pointLightData[i].m_position;
		pointLightData[i].m_position = TBN * lightPos;

		pointLightData[i].m_direction[i] *= TBN;
	}

	for (int i = 0; i < amountOfSpotLightData; i++)
	{
		vec3 lightPos = spotLightData[i].m_position;
		spotLightData[i].m_position = TBN * lightPos;

		spotLightData[i].m_direction[i] *= TBN;
	}*/

	tangentViewPos = TBN * camera_position;
	tangentFragPos = TBN * varying_position;

	gl_Position = combined_xform * aInstanceMatrix * vec4(vertex_position, 1.0);
}