#include "Renderable.hpp"
#include "Engine.hpp"
#include "SubmitModelMessage.hpp"
#include "GetMaterialMessage.hpp"
#include "Material.hpp"

namespace TNAP {

	Renderable::Renderable()
	{
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
	
#if USE_IMGUI
	void TNAP::Renderable::imGuiRenderProperties()
	{
		Entity::imGuiRenderProperties();
		GetMaterialMessage materialMessage;
		materialMessage.m_materialHandle = &m_materialHandles;
		TNAP::getEngine()->sendMessage(&materialMessage);

		for (Material* mat : materialMessage.m_materialVector)
		{
			mat->imGuiRender();
		}
	}
#endif
	
}
