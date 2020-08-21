#include "PBR.hpp"

#include <array>
#include <fstream>

#include "ImGuiInclude.hpp"
#include "Engine.hpp"
#include "GetTextureMessage.hpp"
#include "Utilities.hpp"


namespace TNAP {

	PBR::PBR()
	{
	}

	void PBR::init()
	{
		m_textureHandles.fill(0);

		for (int i = 0; i < static_cast<int>(ETextureType::eCount) - 1; i++)
			m_textureTypes.at(i) = static_cast<ETextureType>(i);
	}

	void PBR::sendShaderData(const GLuint argProgram)
	{
		static std::array<std::string, static_cast<int>(ETextureType::eCount) - 1> m_textureNames
		{
			"material.m_texture",
			"material.m_normalTexture",
			"material.m_metallicTexture",
			"material.m_roughnessTexture",
			"material.m_AOTexture"
		};

		// Send Emission texture (From base Material)
		{
			GetTextureMessage textureMessage({ ETextureType::eEmission, m_emissionTextureHandle });
			getEngine()->sendMessage(&textureMessage);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureMessage.m_textureData->m_textureBinding);

			Helpers::CheckForGLError();
		
			/// Sends Emission Sample to shader
			GLuint emissionTexture_id = glGetUniformLocation(argProgram, "material.m_emissionTexture");
			glUniform1i(emissionTexture_id, 0);

			Helpers::CheckForGLError();

		}
		// Send all PBR textures
		for (int i = 0; i < m_textureHandles.size(); i++)
		{
			GetTextureMessage textureMessage({ m_textureTypes.at(i), m_textureHandles.at(i) });
			getEngine()->sendMessage(&textureMessage);

			glActiveTexture(GL_TEXTURE0 + i + 1);
			glBindTexture(GL_TEXTURE_2D, textureMessage.m_textureData->m_textureBinding);

			Helpers::CheckForGLError();

			/// Sends Texture Sample to shader
			GLuint texture_id = glGetUniformLocation(argProgram, m_textureNames.at(i).c_str());
			glUniform1i(texture_id, i + 1);

			Helpers::CheckForGLError();
		}

		// -------- END OF TEXTURES

		GLuint colourTint_id = glGetUniformLocation(argProgram, "material.m_colourTint");
		glUniform4fv(colourTint_id, 1, glm::value_ptr(m_colourTint));

		GLuint emissionColour_id = glGetUniformLocation(argProgram, "material.m_emissionColour");
		glUniform3fv(emissionColour_id, 1, glm::value_ptr(m_emissionColour));

		GLuint emissionIntensity_id = glGetUniformLocation(argProgram, "material.m_emissionIntensity");
		glUniform1f(emissionIntensity_id, m_emissionIntensity);

		Helpers::CheckForGLError();
	}

	void PBR::saveData(std::ofstream& outputFile, const std::string& argShaderName)
	{
		Material::saveData(outputFile, argShaderName);

		outputFile << ",";

		// Save texture types
		for (int i = 0; i < m_textureHandles.size(); i++)
		{
			if (i < m_textureHandles.size() - 1)
				outputFile << std::to_string(static_cast<int>(m_textureTypes.at(i))) << " ";
			else
				outputFile << std::to_string(static_cast<int>(m_textureTypes.at(i)));
		}

		outputFile << ",";

		// Save texture file paths
		for (int i = 0; i < m_textureHandles.size(); i++)
		{
			GetTextureMessage textureMessage({ m_textureTypes.at(i), m_textureHandles.at(i) });
			getEngine()->sendMessage(&textureMessage);

			if (i < m_textureHandles.size() - 1)
			{
				if (textureMessage.m_textureData->m_filePath.empty())
					outputFile << "EMPTY" << "|";
				else
					outputFile << textureMessage.m_textureData->m_filePath << "|";
			}
			else
			{
				if (textureMessage.m_textureData->m_filePath.empty())
					outputFile << "EMPTY";
				else
					outputFile << textureMessage.m_textureData->m_filePath;
			}
		}
	}

	void PBR::setData(const std::string& argData)
	{
		Material::setData(argData);

		std::vector<std::string> materialData = stringToVector<std::string>(argData, ",", [](const std::string& str) { return str; }, 8);

		{
			// Texture types
			std::vector<ETextureType> textureTypes = stringToVector<ETextureType>(materialData.at(6), " ", [](const std::string& str) { return static_cast<ETextureType>(std::stoi(str)); }, 5);
			setTextureTypes(textureTypes);

			// Texture handles
			std::vector<std::string> textureFilepaths = stringToVector<std::string>(materialData.at(7), "|", [](const std::string& str) { return str; }, 5);
			for (int i = 0; i < textureFilepaths.size(); i++)
			{
				if (i < m_textureHandles.size())
				{
					if ("EMPTY" != textureFilepaths.at(i))
					{
						LoadTextureMessage loadMessage(m_textureTypes.at(i), textureFilepaths.at(i));
						getEngine()->sendMessage(&loadMessage);

						if (loadMessage.m_loadedSuccessfully)
							m_textureHandles.at(i) = loadMessage.m_textureHandle;
					}
				}
			}
		}
	}

	void PBR::setTexture(const ETextureType argType, const std::string& argFilePath)
	{
	}

	inline void PBR::setTextureTypes(const std::vector<ETextureType>& argTextureTypes)
	{
		for (int i = 0; i < m_textureTypes.size(); i++)
			m_textureTypes.at(i) = argTextureTypes.at(i);
	}

	inline void PBR::setTextureHandles(const std::vector<size_t>& argTextureHandles)
	{
		for (int i = 0; i < m_textureHandles.size(); i++)
			m_textureHandles.at(i) = argTextureHandles.at(i);
	}

#if USE_IMGUI
	void PBR::imGuiRender()
	{
		if (ImGui::CollapsingHeader(m_name.c_str()))
		{
			ImGui::Text(("Program Handle: " + std::to_string(m_programHandle)).c_str());

			ImGui::ColorEdit4(("Colour Tint##ColourTintPicker" + m_name).c_str(), &m_colourTint.x);

			ImGui::Checkbox(("Use Transparency##" + m_name).c_str(), &m_useTransparency);
			ImGui::Checkbox(("Double Sided##" + m_name).c_str(), &m_doubleSided);

			for (int i = 0; i < m_textureHandles.size(); i++)
			{
				ImGui::Text(Renderer3D::textureTypeNames.at(i).c_str());
				{
					GetTextureMessage textureMessage({ m_textureTypes.at(i), m_textureHandles.at(i) });
					getEngine()->sendMessage(&textureMessage);

					if (ImGui::ImageButton((ImTextureID)textureMessage.m_textureData->m_textureBinding, ImVec2(32, 32), ImVec2(0, 1), ImVec2(1, 0)))
						m_textureHandles.at(i) = 0;

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_CELL"))
						{
							const std::pair<ETextureType, size_t> data{ *static_cast<const std::pair<ETextureType, size_t>*>(payload->Data) };

							m_textureTypes.at(i) = data.first;
							m_textureHandles.at(i) = data.second;
						}

						ImGui::EndDragDropTarget();
					}
				}
			}

			ImGui::Text(Renderer3D::textureTypeNames.at(static_cast<int>(ETextureType::eEmission)).c_str());
			{
				GetTextureMessage textureMessage({ ETextureType::eEmission, m_emissionTextureHandle });
				getEngine()->sendMessage(&textureMessage);

				if (ImGui::ImageButton((ImTextureID)textureMessage.m_textureData->m_textureBinding, ImVec2(32, 32), ImVec2(0, 1), ImVec2(1, 0)))
					m_emissionTextureHandle = 0;

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TEXTURE_CELL"))
					{
						const std::pair<ETextureType, size_t> data{ *static_cast<const std::pair<ETextureType, size_t>*>(payload->Data) };

						m_emissionTextureHandle = data.second;
					}

					ImGui::EndDragDropTarget();
				}
			}

			ImGui::ColorEdit3(("Emission Colour##EmissionColourPicker" + m_name).c_str(), &m_emissionColour.x);

			ImGui::DragFloat(("Emission Intesity##"+m_name).c_str(), &m_emissionIntensity);
		}
	}
#endif

}