#include "Renderable.hpp"
#include "Engine.hpp"
#include "SubmitModelMessage.hpp"
#include "GetMaterialMessage.hpp"
#include "LoadModelMessage.hpp"
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
			for (int i = 0; i < m_materialHandles.size(); i++)
			{
				ImGui::InputScalar(("[" + std::to_string(i) + "] ").c_str(), ImGuiDataType_::ImGuiDataType_U64, static_cast<void*>(&m_materialHandles.at(i)));
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			for (Material* mat : materialMessage.m_materialVector)
			{
				mat->imGuiRender();
			}
		}

	}
#endif
	
}
