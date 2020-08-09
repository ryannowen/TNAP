#include "Material.hpp"

#include <fstream>

#include "ImGuiInclude.hpp"

#include "Engine.hpp"
#include "Renderer3D.hpp"
#include "GetTextureMessage.hpp"
#include "Utilities.hpp"

TNAP::Material::Material()
{
}

void TNAP::Material::init()
{
}

void TNAP::Material::sendShaderData(const GLuint argProgram)
{
	GetTextureMessage textureMessage({ TNAP::ETextureType::eEmission, m_emissionTextureHandle });
	getEngine()->sendMessage(&textureMessage);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureMessage.m_textureData->m_textureBinding);

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

void TNAP::Material::saveData(std::ofstream& outputFile, const std::string& argShaderName)
{
	outputFile << m_name << ",";
	outputFile << argShaderName << ",";
	outputFile << m_colourTint.r << " " << m_colourTint.g << " " << m_colourTint.b << " " << m_colourTint.a << ",";
	outputFile << m_emissionColour.r << " " << m_emissionColour.g << " " << m_emissionColour.b << "," << m_emissionIntensity;
}

void TNAP::Material::setData(const std::string& argData)
{
	std::vector<std::string> materialData = stringToVector<std::string>(argData, ",", [](const std::string& str) { return str; }, 3);
	
	// Colour Tint
	{
		std::vector<float> colourTint = stringToVector<float>(materialData.at(0), " ", [](const std::string& str) { return std::stof(str); }, 4);
		m_colourTint = { colourTint.at(0), colourTint.at(1), colourTint.at(2), colourTint.at(3) };
	}

	// Emission Colour
	{
		std::vector<float> emissionColour = stringToVector<float>(materialData.at(1), " ", [](const std::string& str) { return std::stof(str); }, 3);
		m_emissionColour = { emissionColour.at(0), emissionColour.at(1), emissionColour.at(2) };
	}

	// Emission Intensity
	m_emissionIntensity = std::stof(materialData.at(2));
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

		{
			GetTextureMessage textureMessage({ ETextureType::eEmission, m_emissionTextureHandle });
			getEngine()->sendMessage(&textureMessage);

			if (ImGui::ImageButton((ImTextureID)textureMessage.m_textureData->m_textureBinding, ImVec2(32, 32), ImVec2(0, 1), ImVec2(1, 0)))
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
		ImGui::SameLine();
		ImGui::Text("Emission");

		ImGui::ColorEdit3("Emission Colour", &m_emissionColour.x);
		ImGui::InputFloat("Emission Intesity", &m_emissionIntensity);
	}
}
#endif