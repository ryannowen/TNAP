#include "Material.hpp"

#include "ImGuiInclude.hpp"

#include "Engine.hpp"
#include "GetTextureMessage.hpp"

TNAP::Material::Material()
{
}

void TNAP::Material::init()
{
}

void TNAP::Material::sendShaderData()
{
}

void TNAP::Material::setEmissionTexture(const std::string& argFilePath)
{
}

void TNAP::Material::setEmissionColour(const glm::vec3& argColour)
{
}

void TNAP::Material::setEmissionIntesity(const float argIntensity)
{
}

#if USE_IMGUI
void TNAP::Material::imGuiRender()
{
	ImGui::Text(("Program being used: " + std::to_string(m_programHandle)).c_str());
	ImGui::ColorEdit4("Colour Tint", &m_colourTint.x);
	ImGui::Checkbox("Has emission Texture", &m_hasEmissionTexture);
	
	GetTextureMessage textureMessage({ ETextureType::eEmission, m_emissionTextureHandle });
	getEngine()->sendMessage(&textureMessage);

	ImGui::ImageButton((ImTextureID)textureMessage.m_textureBinding, ImVec2(64, 64));

	ImGui::ColorEdit4("Emission Colour", &m_emissionColour.x);
	ImGui::InputFloat("Emission Intesity", &m_emissionIntesity);
}
#endif