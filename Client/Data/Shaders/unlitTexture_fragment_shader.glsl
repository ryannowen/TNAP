#version 330

in vec3 varying_position;
in vec2 varying_uv;
in vec3 varying_normal;

out vec4 fragment_colour;

struct SMaterial
{
	vec4 m_colourTint;

	sampler2D m_texture;
	sampler2D m_emissionTexture;

	vec3 m_emissionColour;
	float m_emissionIntensity;
};

uniform SMaterial material;

void main(void)
{
	//fragment_colour = vec4(varying_normal, 1.0);
	//fragment_colour = vec4(0, 1, 0, 1);

	fragment_colour = (texture(material.m_emissionTexture, varying_uv) * material.m_colourTint) + ((texture(material.m_emissionTexture, varying_uv) * vec4(material.m_emissionColour, 1)) * material.m_emissionIntensity);
}