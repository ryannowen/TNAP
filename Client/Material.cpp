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

void TNAP::Material::sendShaderData(const GLuint argProgram)
{
	GetTextureMessage textureMessage({ ETextureType::eEmission, m_emissionTextureHandle });
	getEngine()->sendMessage(&textureMessage);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMessage.m_textureBinding);

	Helpers::CheckForGLError();

	GLuint colourTint_id = glGetUniformLocation(argProgram, "material.m_colourTint");
	glUniform4fv(colourTint_id, 1, glm::value_ptr(m_colourTint));

	/// Sends Emission Sample to shader
	GLuint emissionTexture_id = glGetUniformLocation(argProgram, "material.m_emissionTexture");
	glUniform1i(emissionTexture_id, 0);

	GLuint emissionColour_id = glGetUniformLocation(argProgram, "material.m_emissionColour");
	glUniform3fv(emissionColour_id, 1, glm::value_ptr(m_emissionColour));

	GLuint emissionIntensity_id = glGetUniformLocation(argProgram, "material.m_emissionIntensity");
	glUniform1f(emissionIntensity_id, m_emissionIntensity);

	Helpers::CheckForGLError();
}

void TNAP::Material::setEmissionTexture(const std::string& argFilePath)
{
}

#if USE_IMGUI
void TNAP::Material::imGuiRender()
{
	if (ImGui::CollapsingHeader(m_name.c_str()))
	{
		ImGui::Text(("Program Handle: " + std::to_string(m_programHandle)).c_str());

		ImGui::ColorEdit4("Colour Tint", &m_colourTint.x);

		ImGui::Text("Emission");
		{
			GetTextureMessage textureMessage({ ETextureType::eEmission, m_emissionTextureHandle });
			getEngine()->sendMessage(&textureMessage);

			if (ImGui::ImageButton((ImTextureID)textureMessage.m_textureBinding, ImVec2(64, 64)))
			{
				m_emissionTextureHandle = 0;
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_CELL"))
				{
					const std::pair<ETextureType, size_t> data{ *static_cast<const std::pair<ETextureType, size_t>*>(payload->Data) };

					if (ETextureType::eEmission == data.first)
						m_emissionTextureHandle = data.second;
				}

				ImGui::EndDragDropTarget();
			}
		}

		ImGui::ColorEdit4("Emission Colour", &m_emissionColour.x);
		ImGui::InputFloat("Emission Intesity", &m_emissionIntensity);
	}
}
#endif