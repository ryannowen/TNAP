#pragma once

#include "Light.hpp"

namespace TNAP {

	struct SPointLightData : public SLightData
	{
	private:
		static unsigned int s_amountOfPointLightData;

	public:
		float m_range{ 0 };

		virtual void sendLightData(const GLuint argProgram) override
		{
			GLuint position_id = glGetUniformLocation(argProgram, std::string("pointLightData[" + std::to_string(s_amountOfPointLightData) + "].m_position").c_str());
			glUniform3fv(position_id, 1, glm::value_ptr(m_transform.getTranslation()));

			GLuint colour_id = glGetUniformLocation(argProgram, std::string("pointLightData[" + std::to_string(s_amountOfPointLightData) + "].m_colour").c_str());
			glUniform3fv(colour_id, 1, glm::value_ptr(m_colour));

			GLuint intensity_id = glGetUniformLocation(argProgram, std::string("pointLightData[" + std::to_string(s_amountOfPointLightData) + "].m_intensity").c_str());
			glUniform1f(intensity_id, m_intensity);

			GLuint range_id = glGetUniformLocation(argProgram, std::string("pointLightData[" + std::to_string(s_amountOfPointLightData) + "].m_range").c_str());
			glUniform1f(range_id, m_range);

			/// Increases amount of lights for next light sending
			s_amountOfPointLightData++;
		}

		static void sendAmountOfLights(const GLuint argProgram)
		{
			/// Create  ID and then Sends  data to shader as Uniform
			GLuint numOfLights_id = glGetUniformLocation(argProgram, "amountOfPointLightData");
			glUniform1ui(numOfLights_id, s_amountOfPointLightData);

			s_amountOfPointLightData = 0;
		}
	};

	class PointLight : public TNAP::Light
	{
	protected:
		float m_range{ 100 };

	public:
		PointLight();
		~PointLight();

		virtual void update(const glm::mat4& parentTransform) override;

		inline void setRange(const float argRange) { m_range = argRange; }
		inline const float getRange() const { return m_range; }

#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
