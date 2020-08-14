#include "SpotLight.hpp"

#include "Engine.hpp"
#include "SubmitLightDataMessage.hpp"

namespace TNAP {

	unsigned int SSpotLightData::s_amountOfSpotLightData{ 0 };


	TNAP::SpotLight::SpotLight()
		: PointLight()
	{
	}

	SpotLight::~SpotLight()
	{
	}

	void SpotLight::update(const glm::mat4& parentTransform)
	{
		std::unique_ptr<SSpotLightData> lightData{ std::make_unique<SSpotLightData>() };

		lightData->m_transform = getTransform();
		lightData->m_colour = m_colour;
		lightData->m_intensity = m_intensity;
		lightData->m_range = m_range;
		lightData->m_fov = m_fov;

		SubmitLightDataMessage lightDataMessage;
		lightDataMessage.m_data = std::move(lightData);

		getEngine()->sendMessage(&lightDataMessage);
	}

#if USE_IMGUI
	void SpotLight::imGuiRenderProperties()
	{
		PointLight::imGuiRenderProperties();

		ImGui::DragFloat("FOV", &m_fov, 0.1f);

	}
#endif

}
