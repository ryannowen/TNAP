#pragma once

#include "PointLight.hpp"
#include "ImGuiInclude.hpp"
namespace TNAP {

	struct SSpotLightData : public SPointLightData
	{
	private:
		static unsigned int s_amountOfSpotLightData;

	public:
		float m_fov{ 0 };

		virtual void sendLightData(const GLuint argProgram) override final
		{
			GLuint position_id = glGetUniformLocation(argProgram, std::string("spotLightData[" + std::to_string(s_amountOfSpotLightData) + "].m_position").c_str());
			glUniform3fv(position_id, 1, glm::value_ptr(m_transform.getTranslation()));

			GLuint direction_id = glGetUniformLocation(argProgram, std::string("spotLightData[" + std::to_string(s_amountOfSpotLightData) + "].m_direction").c_str());
			glUniform3fv(direction_id, 1, glm::value_ptr(m_transform.getForwardAxis()));

			GLuint colour_id = glGetUniformLocation(argProgram, std::string("spotLightData[" + std::to_string(s_amountOfSpotLightData) + "].m_colour").c_str());
			glUniform3fv(colour_id, 1, glm::value_ptr(m_colour));

			GLuint intensity_id = glGetUniformLocation(argProgram, std::string("spotLightData[" + std::to_string(s_amountOfSpotLightData) + "].m_intensity").c_str());
			glUniform1f(intensity_id, m_intensity);

			GLuint range_id = glGetUniformLocation(argProgram, std::string("spotLightData[" + std::to_string(s_amountOfSpotLightData) + "].m_range").c_str());
			glUniform1f(range_id, m_range);

			GLuint fov_id = glGetUniformLocation(argProgram, std::string("spotLightData[" + std::to_string(s_amountOfSpotLightData) + "].m_fov").c_str());
			glUniform1f(fov_id, m_fov);

			/// Increases amount of lights for next light sending
			s_amountOfSpotLightData++;
		}

		static void sendAmountOfLights(const GLuint argProgram)
		{
			/// Create  ID and then Sends  data to shader as Uniform
			GLuint numOfLights_id = glGetUniformLocation(argProgram, "amountOfSpotLightData");
			glUniform1ui(numOfLights_id, s_amountOfSpotLightData);

			s_amountOfSpotLightData = 0;
		}
	};

	class SpotLight : public PointLight
	{
	private:
		float m_fov{ 10 };

	public:
		SpotLight();
		~SpotLight();

		virtual void update(const glm::mat4& parentTransform) override;

		inline void setFov(const float argFov) { m_fov = argFov; }
		inline const float getFov() const { return m_fov; }

#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
