#include "Renderable.hpp"
#include "Engine.hpp"
#include "SubmitModelMessage.hpp"
#include "GetMaterialMessage.hpp"
#include "LoadModelMessage.hpp"
#include "GenerateMaterialMessage.hpp"
#include "Material.hpp"

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

	void Renderable::loadModel(const std::string& argFilepath)
	{
		LoadModelMessage loadMessage;
		loadMessage.m_modelFilepath = argFilepath;
		Engine::getInstance()->sendMessage(&loadMessage);
		m_modelHandle = loadMessage.m_modelHandle;
		m_materialHandles = loadMessage.m_materialHandles;
	}
	
#if USE_IMGUI
	void TNAP::Renderable::imGuiRenderProperties()
	{
		Entity::imGuiRenderProperties();
		GetMaterialMessage materialMessage;
		materialMessage.m_materialHandle = &m_materialHandles;
		TNAP::getEngine()->sendMessage(&materialMessage);

		ImGui::InputScalar("Model Handle", ImGuiDataType_::ImGuiDataType_U64, static_cast<void*>(&m_modelHandle));

		ImGui::Spacing();

		if(ImGui::CollapsingHeader("Materials"))
		{
			static size_t materialHandle{ 0 };
			ImGui::InputScalar("Material Handle", ImGuiDataType_::ImGuiDataType_U64, &materialHandle);
			static std::string materialTypeName{ "Unlit" };
			static TNAP::EMaterialType materialType{ TNAP::EMaterialType::eUnlit };
			if(ImGui::BeginCombo("Material Type", materialTypeName.c_str()))
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

			if (ImGui::Button("Generate New Material", ImVec2(ImGui::GetContentRegionAvailWidth(), 20)))
			{
				GenerateMaterialMessage genMessage(materialMessage.m_materialVector.at(materialHandle)->getName(), m_materialHandles.at(materialHandle), materialType);
				TNAP::getEngine()->sendMessage(&genMessage);
			}


			if (materialHandle >= m_materialHandles.size())
				materialHandle = m_materialHandles.size() - 1;
			else if (materialHandle < 0)
				materialHandle = 0;

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Material Handles");
			for (int i = 0; i < m_materialHandles.size(); i++)
			{
				ImGui::InputScalar(("[" + std::to_string(i) + "] ").c_str(), ImGuiDataType_::ImGuiDataType_U64, static_cast<void*>(&m_materialHandles.at(i)));
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			for (Material* const mat : materialMessage.m_materialVector)
			{
				mat->imGuiRender();
			}
		}

	}
#endif
	
}
