#version 430

//layout(early_fragment_tests) in;

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
	sampler2D m_normalTexture;
	sampler2D m_metallicTexture;
	sampler2D m_roughnessTexture;
	sampler2D m_AOTexture;

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

uniform sampler2D depthMap;
uniform vec2 viewportSize;

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

//in vec3 varying_position;
in vec2 varying_uv;
//in vec3 varying_normal;

in vec3 tangentViewPos;
in vec3 tangentFragPos;

in mat3 varying_TBN;

out vec4 fragment_colour;

const float calculateAttenuation(const vec3 argPosition, const float argRange)
{
	float light_distance = distance(argPosition, tangentFragPos);
	return (1.0 - smoothstep(0, argRange, light_distance));
}

const vec3 calculateBasicLight(const vec3 argNormal, const vec3 argDirection, const vec3 argColour, const float argIntensity)
{
	vec3 lightDirection = normalize(argDirection);

	vec3 diffuse_intensity = max(vec3(0.0), dot(lightDirection, normalize(argNormal)));
	diffuse_intensity *= max(vec3(0.1), argColour) * max(0.01, argIntensity);

	return diffuse_intensity;
}

const vec3 calculatePointLight(const vec3 argNormal, const vec3 argDirection, const vec3 argColour, const float argIntensity, const vec3 argPosition, const float argRange)
{
	vec3 diffuse_intensity = calculateBasicLight(argNormal, argPosition - tangentFragPos, argColour, argIntensity);
	float attenuation = calculateAttenuation(argPosition, argRange);

	return (diffuse_intensity * (attenuation * attenuation));
}

const vec3 calculateSpotLight(const vec3 argNormal, const vec3 argDirection, const vec3 argColour, const float argIntensity, const vec3 argPosition, const float argRange, const float argFOV)
{
	vec3 diffuse_intensity = calculatePointLight(argNormal, vec3(0), argColour, argIntensity, argPosition, argRange);
	float attenuation = calculateAttenuation(argPosition, argRange);

	vec3 lightDirection = normalize(argPosition - tangentFragPos);
	attenuation *= smoothstep(cos(0.5 * radians(argFOV)), 1, dot(-lightDirection, normalize(argDirection)));

	return (diffuse_intensity * attenuation);
}

const vec4 applyHDR(vec4 argApplicant)
{
	// exposure tone mapping
	argApplicant.rgb = vec3(1.0) - exp(-argApplicant.rgb * sceneData.m_exposure);
	// gamma correction 
	argApplicant.rgb = pow(argApplicant.rgb, vec3(1.0 / sceneData.m_gamma));

	return argApplicant;
}

void main(void)
{
	vec2 fragCoord = gl_FragCoord.xy / viewportSize;

	//vec3 depth = texture(depthStencil, fragCoord).rrr;
	float depth = texture(depthMap, fragCoord).r;
	if (gl_FragCoord.z > depth)
		discard;

	vec4 finalResult = vec4(0, 0, 0, 1);
	vec3 normal = texture(material.m_normalTexture, varying_uv).rgb;

	normal = normalize(normal * 2 - 1);

	// Apply Basic Lights (Directional)
	for (uint i = uint(0); ((i < amountOfLightData) && (i < BASIC_LIGHT_AMOUNT)); i++)
		finalResult += vec4(calculateBasicLight(normal, -(varying_TBN * lightData[i].m_direction), lightData[i].m_colour, lightData[i].m_intensity), 0);

	// Apply Point Lights
	for (uint i = uint(0); ((i < amountOfPointLightData) && (i < POINT_LIGHT_AMOUNT)); i++)
		finalResult += vec4(calculatePointLight(normal, vec3(0), pointLightData[i].m_colour, pointLightData[i].m_intensity, varying_TBN * pointLightData[i].m_position, pointLightData[i].m_range), 0);
	
	// Apply Spot Lights
	for (uint i = uint(0); ((i < amountOfSpotLightData) && (i < SPOT_LIGHT_AMOUNT)); i++)
		finalResult += vec4(calculateSpotLight(normal, vec3(0), spotLightData[i].m_colour, spotLightData[i].m_intensity, varying_TBN * spotLightData[i].m_position, spotLightData[i].m_range, spotLightData[i].m_fov), 0);

	// Add Textures
	finalResult *= (texture(material.m_texture, varying_uv) * material.m_colourTint) + ((texture(material.m_emissionTexture, varying_uv) * vec4(material.m_emissionColour, 1)) * material.m_emissionIntensity);
	
	// Add Ambient
	finalResult += vec4(sceneData.m_ambientColour * sceneData.m_ambientIntensity, 0);

	// Convert to HDR
	finalResult = applyHDR(finalResult);

	// Output Result
	fragment_colour = finalResult;
}