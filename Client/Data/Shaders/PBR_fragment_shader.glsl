#version 330

in vec3 varying_position;
in vec2 varying_uv;
in vec3 varying_normal;

out vec4 fragment_colour;

struct SLightData
{
	vec3 m_direction;
	vec3 m_colour;
	float m_intensity;
};

struct SPointLightData
{
	vec3 m_position;
	vec3 m_colour;
	float m_intensity;

	float m_range;
};

struct SSpotLightData
{
	vec3 m_position;
	vec3 m_direction;
	vec3 m_colour;
	float m_intensity;

	float m_range;

	float m_fov;
};

struct SMaterial
{
	vec4 m_colourTint;

	sampler2D m_texture;
	sampler2D m_emissionTexture;

	vec3 m_emissionColour;
	float m_emissionIntensity;
};

struct SSceneData
{
	vec3 m_ambientColour;
	float m_ambientIntensity;
	float m_exposure;
	float m_gamma;
};

uniform SMaterial material;

// Light Arrays
#define BASIC_LIGHT_AMOUNT uint(10)
#define POINT_LIGHT_AMOUNT uint(50)
#define SPOT_LIGHT_AMOUNT uint(50)

uniform SLightData lightData[BASIC_LIGHT_AMOUNT];
uniform uint amountOfLightData;

uniform SPointLightData pointLightData[POINT_LIGHT_AMOUNT];
uniform uint amountOfPointLightData;

uniform SSpotLightData spotLightData[SPOT_LIGHT_AMOUNT];
uniform uint amountOfSpotLightData;

uniform SSceneData sceneData;

const vec3 calculateBasicLight(const SLightData argLightData)
{
	vec3 lightDirection = normalize(-argLightData.m_direction);

	vec3 diffuse_intensity = max(vec3(0.0), dot(lightDirection, normalize(varying_normal)));
	diffuse_intensity *= max(vec3(0.1), argLightData.m_colour) * max(0.01, argLightData.m_intensity);

	return diffuse_intensity;
}

const vec3 calculatePointLight(const SPointLightData argPointLightData)
{
	vec3 lightDirection = normalize(argPointLightData.m_position - varying_position);

	vec3 diffuse_intensity = max(vec3(0.0), dot(lightDirection, normalize(varying_normal)));
	diffuse_intensity *= max(vec3(0.1), argPointLightData.m_colour) * max(0.01, argPointLightData.m_intensity);

	// Attenuation
	float light_distance = distance(argPointLightData.m_position, varying_position);
	float attenuation = 1.0 - smoothstep(0, argPointLightData.m_range, light_distance);

	return (diffuse_intensity * attenuation);
}

const vec3 calculateSpotLight(const SSpotLightData argSpotLightData)
{
	vec3 lightDirection = normalize(argSpotLightData.m_position - varying_position);

	vec3 diffuse_intensity = max(vec3(0.0), dot(lightDirection, normalize(varying_normal)));
	diffuse_intensity *= max(vec3(0.1), argSpotLightData.m_colour) * max(0.01, argSpotLightData.m_intensity);

	// Attenuation
	float light_distance = distance(argSpotLightData.m_position, varying_position);
	float attenuation = 1.0 - smoothstep(0, argSpotLightData.m_range, light_distance);

	attenuation *= smoothstep(cos(0.5 * radians(argSpotLightData.m_fov)), 1, dot(-lightDirection, normalize(argSpotLightData.m_direction)));

	return (diffuse_intensity * attenuation);
}

void main(void)
{
	vec4 finalResult = vec4(0);

	for (uint i = uint(0); i < amountOfLightData; i++)
	{
		if (i < BASIC_LIGHT_AMOUNT)
			finalResult += vec4(calculateBasicLight(lightData[i]), 0);
	}

	for (uint i = uint(0); i < amountOfPointLightData; i++)
	{
		if (i < POINT_LIGHT_AMOUNT)
			finalResult += vec4(calculatePointLight(pointLightData[i]), 0);
	}

	for (uint i = uint(0); i < amountOfSpotLightData; i++)
	{
		if (i < SPOT_LIGHT_AMOUNT)
			finalResult += vec4(calculateSpotLight(spotLightData[i]), 0);
	}

	//finalResult *= finalResult * material.m_colourTint.rgb;
	finalResult *= (texture(material.m_texture, varying_uv) * material.m_colourTint) + ((texture(material.m_emissionTexture, varying_uv) * vec4(material.m_emissionColour, 1)) * material.m_emissionIntensity);

	finalResult += vec4(sceneData.m_ambientColour, 1) * sceneData.m_ambientIntensity;

	// exposure tone mapping
	finalResult.rgb = vec3(1.0) - exp(-finalResult.rgb * sceneData.m_exposure);
	// gamma correction 
	finalResult.rgb = pow(finalResult.rgb, vec3(1.0 / sceneData.m_gamma));

	fragment_colour = finalResult;
}