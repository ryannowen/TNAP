#include "Renderable.hpp"
#include "Engine.hpp"
#include "SubmitModelMessage.hpp"
#include "GetMaterialMessage.hpp"
#include "LoadModelMessage.hpp"
#include "GenerateMaterialMessage.hpp"
#include "GetModelInfoMessage.hpp"
#include "Material.hpp"
#include <fstream>

namespace TNAP {

	Renderable::Renderable() 
		: Entity()
	{
	}

	Renderable::Renderable(const std::string& argFilepath) 
		: Entity()
	{
		loadModel(argFilepath);
	}

	Renderable::~Renderable()
	{
	}

	void TNAP::Renderable::init()
	{
	}

	void TNAP::Renderable::update(const glm::mat4& parentTransform)
	{
		if (!getEnabled())
			return;

		SubmitModelMessage submitMessage;
		submitMessage.m_modelHandle = m_modelHandle;
		submitMessage.m_materialHandle = &m_materialHandles;
		submitMessage.m_transform = parentTransform * getTransform().getMatrix();
		Engine::getInstance()->sendMessage(&submitMessage);

		Entity::update(parentTransform);

	}

	void Renderable::saveData(std::ofstream& outputFile)
	{
		Entity::saveData(outputFile);

		GetModelInfoMessage modelInfoMessage;
		modelInfoMessage.m_modelHandle = m_modelHandle;
		TNAP::getEngine()->sendMessage(&modelInfoMessage);
		outputFile << "," << modelInfoMessage.m_filepath << ",";

		GetMaterialMessage message;
		message.m_materialHandle = &m_materialHandles;
		TNAP::getEngine()->sendMessage(&message);
		for (size_t i = 0; i < message.m_materialVector.size(); i++)
		{
			if (i < message.m_materialVector.size()-1)
				outputFile << message.m_materialVector.at(i)->getName() << " "; 
			else
				outputFile << message.m_materialVector.at(i)->getName();
		}
	}

	void Renderable::loadModel(const std::string& argFilepath)
	{
		LoadModelMessage loadMessage;
		loadMessage.m_modelFilepath = argFilepath;
		TNAP::getEngine()->sendMessage(&loadMessage);
		m_modelHandle = loadMessage.m_modelHandle;
		m_materialHandles = loadMessage.m_materialHandles;
	}

	void Renderable::setMaterialHandles(const std::vector<size_t>& argMaterialHandles)
	{
		m_materialHandles.clear();

		for (const size_t handle : argMaterialHandles)
			m_materialHandles.push_back(handle);
	}
	
#if USE_IMGUI
	void TNAP::Renderable::imGuiRenderProperties()
	{
		Entity::imGuiRenderProperties();
		GetMaterialMessage materialMessage;
		materialMessage.m_materialHandle = &m_materialHandles;
		TNAP::getEngine()->sendMessage(&materialMessage);

		ImGui::ImageButton((ImTextureID)0, ImVec2(16, 16));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MODEL_CELL"))
			{
				const size_t data{ *static_cast<size_t*>(payload->Data) };

				m_modelHandle = data;
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::SameLine();
		ImGui::InputScalar("Model Handle", ImGuiDataType_::ImGuiDataType_U64, static_cast<void*>(&m_modelHandle));

		ImGui::Spacing();

		if(ImGui::CollapsingHeader("Materials"))
		{
			static size_t materialIndex{ 0 };
			static std::string materialName{ "" };
			static std::string replaceMaterialName{ "" };

			if (ImGui::CollapsingHeader("Material Generation"))
			{
				ImGui::InputText("New Material Name", &materialName);

				if (ImGui::BeginCombo("Replace Material", replaceMaterialName.c_str()))
				{
					GetMaterialMessage getMatMessage;
					getMatMessage.m_materialHandle = &m_materialHandles;
					TNAP::getEngine()->sendMessage(&getMatMessage);

					for (int i = 0; i < getMatMessage.m_materialVector.size(); i++)
					{
						if (ImGui::Selectable(getMatMessage.m_materialVector.at(i)->getName().c_str()))
						{
							replaceMaterialName = getMatMessage.m_materialVector.at(i)->getName().c_str();
							materialIndex = i;
						}
					}


					ImGui::EndCombo();
				}

				static std::string materialTypeName{ "Unlit" };
				static TNAP::EMaterialType materialType{ TNAP::EMaterialType::eUnlit };
				if (ImGui::BeginCombo("Material Type", materialTypeName.c_str()))
				{
					if (ImGui::Selectable("Unlit"))
					{
						materialTypeName = "Unlit";
						materialType = TNAP::EMaterialType::eUnlit;
					}
					if (ImGui::Selectable("UnlitTexture"))
					{
						materialTypeName = "UnlitTexture";
						materialType = TNAP::EMaterialType::eUnlitTexture;
					}
					if (ImGui::Selectable("PBR"))
					{
						materialTypeName = "PBR";
						materialType = TNAP::EMaterialType::ePBR;
					}

					ImGui::EndCombo();
				}

				if (materialIndex >= m_materialHandles.size())
					materialIndex = m_materialHandles.size() - 1;
				else if (materialIndex < 0)
					materialIndex = 0;

				if (ImGui::Button("Generate New Material", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)) && !replaceMaterialName.empty())
				{
					if (materialName.empty())
						materialName = materialMessage.m_materialVector.at(materialIndex)->getName();

					GenerateMaterialMessage genMessage(materialName, m_materialHandles.at(materialIndex), materialType);
					TNAP::getEngine()->sendMessage(&genMessage);
					replaceMaterialName.clear();
					materialName.clear();

				}
			}
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Material Handles"))
			{
				for (int i = 0; i < m_materialHandles.size(); i++)
				{
					ImGui::ImageButton((ImTextureID)0, ImVec2(16, 16));
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL_CELL"))
						{
							const size_t data{ *static_cast<size_t*>(payload->Data) };

							m_materialHandles.at(i) = data;


						}

						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();
					ImGui::InputScalar(("[" + std::to_string(i) + "] ").c_str(), ImGuiDataType_::ImGuiDataType_U64, static_cast<void*>(&m_materialHandles.at(i)));
				}
			}

			ImGui::Spacing();

			for (Material* const mat : materialMessage.m_materialVector)
			{
				mat->imGuiRender();
				ImGui::Spacing();
			}
		}

	}
#endif
	
}
