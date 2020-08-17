#include "PBR.hpp"

#include <array>

#include "ImGuiInclude.hpp"
#include "Engine.hpp"
#include "GetTextureMessage.hpp"


namespace TNAP {

	PBR::PBR()
	{
	}

	void PBR::init()
	{
		m_textureHandles.fill(0);
	}

	void PBR::sendShaderData(const GLuint argProgram)
	{
		static std::array<std::string, static_cast<int>(ETextureType::eCount) - 1> m_textureNames
		{
			"m_texture",
			"m_normalTexture",
			"m_metallicTexture",
			"m_roughnessTexture",
			"m_AOTexture"
		};

		// Send all PBR textures
		for (int i = 0; i < m_textureHandles.size(); i++)
		{
			GetTextureMessage textureMessage({ static_cast<TNAP::ETextureType>(i), m_textureHandles.at(i) });
			getEngine()->sendMessage(&textureMessage);

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureMessage.m_textureData->m_textureBinding);

			Helpers::CheckForGLError();

			/// Sends Metallic Texture Sample to shader
			GLuint texture_id = glGetUniformLocation(argProgram, m_textureNames.at(i).c_str());
			glUniform1i(texture_id, i);

			Helpers::CheckForGLError();
		}

		// Send Emission texture (From base Material)
		{
			GetTextureMessage textureMessage({ ETextureType::eEmission, m_emissionTextureHandle });
			getEngine()->sendMessage(&textureMessage);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureMessage.m_textureData->m_textureBinding);

			Helpers::CheckForGLError();
		}

		/// Sends Emission Sample to shader
		GLuint emissionTexture_id = glGetUniformLocation(argProgram, "material.m_emissionTexture");
		glUniform1i(emissionTexture_id, 2);

		// -------- END OF TEXTURES

		GLuint colourTint_id = glGetUniformLocation(argProgram, "material.m_colourTint");
		glUniform4fv(colourTint_id, 1, glm::value_ptr(m_colourTint));

		GLuint emissionColour_id = glGetUniformLocation(argProgram, "material.m_emissionColour");
		glUniform3fv(emissionColour_id, 1, glm::value_ptr(m_emissionColour));

		GLuint emissionIntensity_id = glGetUniformLocation(argProgram, "material.m_emissionIntensity");
		glUniform1f(emissionIntensity_id, m_emissionIntensity);

		Helpers::CheckForGLError();
	}

	void PBR::setTexture(const ETextureType argType, const std::string& argFilePath)
	{
	}

#if USE_IMGUI
	void PBR::imGuiRender()
	{
		if (ImGui::CollapsingHeader(m_name.c_str()))
		{
			ImGui::Text(("Program Handle: " + std::to_string(m_programHandle)).c_str());

			ImGui::ColorEdit4(("Colour Tint##ColourTintPicker" + m_name).c_str(), &m_colourTint.x);

			for (int i = 0; i < Renderer3D::textureTypeNames.size(); i++)
			{
				ImGui::Text(Renderer3D::textureTypeNames.at(i).c_str());
				{
					GetTextureMessage textureMessage({ static_cast<ETextureType>(i), m_textureHandles.at(i) });
					getEngine()->sendMessage(&textureMessage);

					if (ImGui::ImageButton((ImTextureID)textureMessage.m_textureData->m_textureBinding, ImVec2(32, 32), ImVec2(0, 1), ImVec2(1, 0)))
					{
						m_textureHandles.at(i) = 0;
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_CELL"))
						{
							const std::pair<ETextureType, size_t> data{ *static_cast<const std::pair<ETextureType, size_t>*>(payload->Data) };

							m_textureHandles.at(i) = data.second;
						}

						ImGui::EndDragDropTarget();
					}
				}
			}

			ImGui::ColorEdit3(("Emission Colour##EmissionColourPicker" + m_name).c_str(), &m_emissionColour.x);

			ImGui::InputFloat("Emission Intesity", &m_emissionIntensity);
		}
	}
#endif

}