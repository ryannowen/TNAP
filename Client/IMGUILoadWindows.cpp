#include "IMGUILoadWindows.hpp"

#include <array>

#include "Engine.hpp"
#include "Renderer3D.hpp"

namespace TNAP {

	//
	// Model Load
	//

	void IMGUIModelLoad::ImGuiRenderLoad(const size_t argLoadWindowID)
	{
#if USE_IMGUI
		ImGuiWindowFlags flags{ ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking };
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar;

		ImGui::SetNextWindowSize(ImVec2(300, 100));
		ImGui::Begin(("Load Model##" + std::to_string(argLoadWindowID)).c_str(), NULL, flags);
		{
			ImGui::InputText("Name", &m_loadMessage.m_modelName);
			ImGui::Text(m_loadMessage.m_modelFilepath.c_str());

			if (ImGui::Button("Load"))
			{
				if (!m_loadMessage.m_modelFilepath.empty())
				{
					getEngine()->sendMessage(&m_loadMessage);
					m_shouldClose = true;
				}
			}
			else
			{
				ImGui::SameLine();

				m_shouldClose = ImGui::Button("Cancel");
			}
		}
		ImGui::End();
#endif
	}

	// 
	//	Texture load
	//

	void IMGUITextureLoad::ImGuiRenderLoad(const size_t argLoadWindowID)
	{
#if USE_IMGUI

		ImGuiWindowFlags flags{ ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking };
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings;
		flags |= ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar;

		ImGui::SetNextWindowSize(ImVec2(300, 150));
		ImGui::Begin(("Load Model##" + std::to_string(argLoadWindowID)).c_str(), NULL, flags);
		{
			ImGui::InputText("Name", &m_loadMessage.m_textureName);
			ImGui::Text(m_loadMessage.m_loadInfo.second.c_str());

			if (ImGui::BeginCombo("Texture Type", Renderer3D::textureTypeNames.at(static_cast<int>(m_loadMessage.m_loadInfo.first)).c_str()))
			{
				for (int i = 0; i < Renderer3D::textureTypeNames.size(); i++)
				{
					if (ImGui::Selectable(Renderer3D::textureTypeNames.at(i).c_str()))
						m_loadMessage.m_loadInfo.first = static_cast<TNAP::ETextureType>(i);
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Load"))
			{
				if (!m_loadMessage.m_loadInfo.second.empty())
				{
					getEngine()->sendMessage(&m_loadMessage);
					m_shouldClose = true;
				}
			}
			else
			{
				ImGui::SameLine();

				m_shouldClose = ImGui::Button("Cancel");
			}
		}
		ImGui::End();
#endif
	}
}