#include "PointLight.hpp"

#include "Engine.hpp"
#include "SubmitLightDataMessage.hpp"

namespace TNAP {

	unsigned int SPointLightData::s_amountOfPointLightData{ 0 };


	PointLight::PointLight() 
		: Light()
	{
	}

	PointLight::~PointLight()
	{
	}

	void PointLight::update(const glm::mat4& parentTransform)
	{
		std::unique_ptr<SPointLightData> lightData{ std::make_unique<SPointLightData>() };

		lightData->m_transform = getTransform();
		lightData->m_colour = m_colour;
		lightData->m_intensity = m_intensity;
		lightData->m_range = m_range;

		SubmitLightDataMessage lightDataMessage;
		lightDataMessage.m_data = std::move(lightData);

		getEngine()->sendMessage(&lightDataMessage);
	}

#if USE_IMGUI
	void PointLight::imGuiRenderProperties()
	{
		Light::imGuiRenderProperties();

		ImGui::DragFloat("Range", &m_range, 0.1f);

	}
#endif

}
