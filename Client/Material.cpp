#include "Material.hpp"

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
}
#endif