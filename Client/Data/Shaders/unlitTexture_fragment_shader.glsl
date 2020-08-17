#version 330

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
uniform SSceneData sceneData;

in vec3 varying_position;
in vec2 varying_uv;
in vec3 varying_normal;

out vec4 fragment_colour;

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
	//fragment_colour = vec4(varying_normal, 1.0);
	//fragment_colour = vec4(0, 1, 0, 1);
	vec4 finalResult = vec4(0, 0, 0, 1);

	// Add Textures
	finalResult = (texture(material.m_texture, varying_uv) * material.m_colourTint) + ((texture(material.m_emissionTexture, varying_uv) * vec4(material.m_emissionColour, 1)) * material.m_emissionIntensity);

	// Add Ambient
	finalResult += vec4(sceneData.m_ambientColour, 0) * sceneData.m_ambientIntensity;

	// Convert to HDR
	finalResult = applyHDR(finalResult);

	fragment_colour = finalResult;
}