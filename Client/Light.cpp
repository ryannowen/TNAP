#include "Light.hpp"

#include <fstream>

#include "Engine.hpp"
#include "SubmitLightDataMessage.hpp"

namespace TNAP {

	unsigned int SLightData::s_amountOfLightData{ 0 };

	Light::Light() 
		: Entity()
	{
	}

	Light::~Light()
	{
	}

	void Light::update(const glm::mat4& parentTransform)
	{
		std::unique_ptr<SLightData> lightData{ std::make_unique<SLightData>() };

		lightData->m_transform = getTransform();
		lightData->m_colour = m_colour;
		lightData->m_intensity = m_intensity;

		SubmitLightDataMessage lightDataMessage;
		lightDataMessage.m_data = std::move(lightData);

		getEngine()->sendMessage(&lightDataMessage);

		Entity::update(parentTransform);
	}

	void Light::saveData(std::ofstream& outputFile)
	{
		Entity::saveData(outputFile);

		outputFile << "," << m_colour.x << " " << m_colour.y << " " << m_colour.z << ",";
		outputFile << m_intensity;

		if (getEntityType() == EEntityType::eLight)
			outputFile << std::endl;
	}

#if USE_IMGUI
	void Light::imGuiRenderProperties()
	{
		Entity::imGuiRenderProperties();

		ImGui::ColorEdit3("Colour", &m_colour.x);

		ImGui::DragFloat("Intensity", &m_intensity, 0.1f);
	}
#endif

}