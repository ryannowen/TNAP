#pragma once

#include "Entity.hpp"
#include "ExternalLibraryHeaders.h"

namespace TNAP {

	struct SLightData
	{
	private:
		static unsigned int s_amountOfLightData;

	public:
		Transform m_transform;
		glm::vec3 m_colour{ 1, 1, 1 };
		float m_intensity{ 1 };

		virtual void sendLightData(const GLuint argProgram)
		{
			GLuint direction_id = glGetUniformLocation(argProgram, std::string("lightData[" + std::to_string(s_amountOfLightData) + "].m_direction").c_str());
			glUniform3fv(direction_id, 1, glm::value_ptr(m_transform.getForwardAxis()));

			GLuint colour_id = glGetUniformLocation(argProgram, std::string("lightData[" + std::to_string(s_amountOfLightData) + "].m_colour").c_str());
			glUniform3fv(colour_id, 1, glm::value_ptr(m_colour));

			GLuint intensity_id = glGetUniformLocation(argProgram, std::string("lightData[" + std::to_string(s_amountOfLightData) + "].m_intensity").c_str());
			glUniform1f(intensity_id, m_intensity);

			/// Increases amount of lights for next light sending
			s_amountOfLightData++;
		}

		static void sendAmountOfLights(const GLuint argProgram)
		{
			/// Create  ID and then Sends  data to shader as Uniform
			GLuint numOfLights_id = glGetUniformLocation(argProgram, "amountOfLightData");
			glUniform1ui(numOfLights_id, s_amountOfLightData);

			s_amountOfLightData = 0;
		}

	};

	class Light : public TNAP::Entity
	{
	protected:
		glm::vec3 m_colour{ 1, 1, 1 };
		float m_intensity{ 1 };

	public:
		Light();
		~Light();
		virtual void update(const glm::mat4& parentTransform) override;

		inline void setColour(const glm::vec3& argColour) { m_colour = argColour; }
		inline const glm::vec3& getColour() const { return m_colour; }

		inline void setIntensity(const float argIntensity) { m_intensity = argIntensity; }
		inline const float getIntensity() const { return m_intensity; }


#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
