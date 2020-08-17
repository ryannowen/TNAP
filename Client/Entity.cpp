#include "Entity.hpp"

#include <memory>
#include <fstream>

#include "ImGuiInclude.hpp"

namespace TNAP {

#if USE_IMGUI
	Entity* Entity::s_selected{ nullptr };
#endif

	void Entity::updateChildrenParentHandles()
	{
		for (const size_t entityHandle : m_children)
		{
			TNAP::getSceneManager()->getCurrentScene()->findEntity(entityHandle)->setParentHandle(m_handle);
		}
	}

	void Entity::deleteChildren()
	{
		size_t childrenSize { m_children.size() };
		for (size_t i = 0; i < childrenSize; i++)
		{
			TNAP::getSceneManager()->getCurrentScene()->destroyEntity(m_children[0]);
		}
	}

	void Entity::removeChild(const size_t argHandle)
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (argHandle == m_children.at(i))
			{
				std::swap(m_children.at(i), m_children.back());
				m_children.pop_back();
				break;
			}
		}
	}

	void Entity::replaceChildHandle(const size_t argOld, const size_t argNew)
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (argOld == m_children.at(i))
			{
				m_children.at(i) = argNew;
				break;
			}
		}
	}

	Entity::Entity()
	{
	}

	Entity::~Entity()
	{
	}

	void Entity::init()
	{
	}

	void Entity::update(const glm::mat4& parentTransform)
	{
		for (const size_t entityHandle : m_children)
		{
			if (TNAP::getSceneManager()->getCurrentScene()->findEntity(entityHandle)->getEnabled())
				TNAP::getSceneManager()->getCurrentScene()->findEntity(entityHandle)->update(parentTransform * getTransform().getMatrix());
		}
	}

	void Entity::saveData(std::ofstream& outputFile)
	{
		outputFile << m_name << ",";
		outputFile << static_cast<size_t>(getEntityType()) << ",";
		outputFile << m_enabled << ",";
		outputFile << m_hasParent << ",";
		outputFile << m_parentHandle << ",";
		outputFile << m_transform.getTranslation().x << " " << m_transform.getTranslation().y << " " << m_transform.getTranslation().z << " ";
		outputFile << m_transform.getRotation().x << " " << m_transform.getRotation().y << " " << m_transform.getRotation().z << " ";
		outputFile << m_transform.getScale().x << " " << m_transform.getScale().y << " " << m_transform.getScale().z << ",";
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (i < m_children.size() - 1)
				outputFile << m_children.at(i) << " ";
			else
				outputFile << m_children.at(i);
		}
		if (getEntityType() == EEntityType::eEntity)
			outputFile << std::endl;
	}

	inline bool Entity::setName(const std::string& argName)
	{
		// Entity doesnt have a name yet
		if ("" != argName && "" != m_name)
		{
			if (updateNameInSceneMap(argName))
			{
				m_name = argName;
#if USE_IMGUI
				m_namePlaceholder = m_name;
#endif
				return true;
			}
		}
		if ("" == m_name)
		{
			m_name = argName;
#if USE_IMGUI
			m_namePlaceholder = m_name;
#endif
			return true;
		}
		return false;
	}

	bool Entity::updateNameInSceneMap(const std::string& argNewName)
	{
		return TNAP::getSceneManager()->getCurrentScene()->updateEntityInMap(m_name, argNewName, m_handle);
	}

	void Entity::setChildren(const std::vector<size_t>& argChildrenHandles)
	{
		m_children.clear();

		for (const size_t handle : argChildrenHandles)
			m_children.push_back(handle);
	}

	TNAP::Entity* const Entity::findChild(const size_t argHandle)
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (argHandle == m_children.at(i))
				return TNAP::getSceneManager()->getCurrentScene()->findEntity(argHandle);
		}
	}

	TNAP::Entity* const Entity::findChild(const std::string& argName)
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (TNAP::getSceneManager()->getCurrentScene()->findEntity(argName)->getHandle() == m_children.at(i))
				return TNAP::getSceneManager()->getCurrentScene()->findEntity(argName);
		}
	}

	TNAP::Entity* const Entity::findChildRecursive(const std::string& argName)
	{
		// Search for child in current entity
		for (size_t i = 0; i < m_children.size(); i++)
		{
			// If entity to find is a child of the current entity	
			if (TNAP::getSceneManager()->getCurrentScene()->findEntity(argName)->getHandle() == m_children.at(i))
				return TNAP::getSceneManager()->getCurrentScene()->findEntity(argName);
		}

		// Search fo child in children
		for (size_t i = 0; i < m_children.size(); i++)
		{
			Entity* entity = TNAP::getSceneManager()->getCurrentScene()->findEntity(m_children.at(i))->findChildRecursive(argName);
			if (nullptr != entity)
				return entity;
		}

		return nullptr;
	}

	void Entity::destroySelf()
	{
		TNAP::getSceneManager()->getCurrentScene()->destroyEntity(m_handle);
	}

	void Entity::destroyChild(const std::string& argName)
	{
		
	}

	void Entity::destroyChild(const size_t argHandle)
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (argHandle == m_children.at(i))
			{
				TNAP::getSceneManager()->getCurrentScene()->destroyEntity(argHandle);
				break;
			}
		}
	}

	void Entity::sendMessage(TNAP::Message* const argMessage)
	{
	}

#if USE_IMGUI
	void Entity::imGuiRenderHierarchy()
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		ImGuiTreeNodeFlags node_flags = base_flags;

		if (this == s_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		if (0 == m_children.size())
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool node_open = ImGui::TreeNodeEx((void*)m_name.c_str(), node_flags, m_name.c_str());

		if (ImGui::IsItemClicked())
			s_selected = this;

		if (node_open)
		{
			for (int i = 0; i < m_children.size(); i++)
			{
				TNAP::getSceneManager()->getCurrentScene()->findEntity(m_children[i])->imGuiRenderHierarchy();
			}
			if (0 != m_children.size())
				ImGui::TreePop();
		}
	}

	void Entity::imGuiRenderProperties()
	{
		if (ImGui::InputText("Name", &m_namePlaceholder))
		{
			if ("" != m_namePlaceholder)
			{
				setName(m_namePlaceholder);
			}
		}
		ImGui::Spacing();
		ImGui::Checkbox("Enabled", &m_enabled);
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::DragFloat3("Translation", &getTransform().getTranslation().x, 0.01f);
			ImGui::DragFloat3("Rotation", &getTransform().getRotation().x, 0.01f);
			ImGui::DragFloat3("Scale", &getTransform().getScale().x, 0.01f);
		}
	}
#endif

}