#include "Entity.hpp"
#include "ImGuiInclude.hpp"
#include <memory>
#include <fstream>

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
			{
				TNAP::getSceneManager()->getCurrentScene()->findEntity(entityHandle)->update(parentTransform * getTransform().getMatrix());
			}
		}

		/*
		for (const std::shared_ptr<Entity>& entity : m_children)
		{
			if (entity->getEnabled())
				entity->update(parentTransform * getTransform().getMatrix());
		}
		*/
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
			{
				return TNAP::getSceneManager()->getCurrentScene()->findEntity(argHandle);
			}
		}
	}

	TNAP::Entity* const Entity::findChild(const std::string& argName)
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (TNAP::getSceneManager()->getCurrentScene()->findEntity(argName)->getHandle() == m_children.at(i))
			{
				return TNAP::getSceneManager()->getCurrentScene()->findEntity(argName);
			}
		}
	}

	TNAP::Entity* const Entity::findChildRecursive(const std::string& argName)
	{
		
		// If entity to find is a child of the current entity
		for (size_t i = 0; i < m_children.size(); i++)
		{
			if (TNAP::getSceneManager()->getCurrentScene()->findEntity(argName)->getHandle() == m_children.at(i))
			{
				return TNAP::getSceneManager()->getCurrentScene()->findEntity(argName);
			}
		}

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
		/*
		// Invalid entity name
		if (m_mapChildEntities.find(argName) == m_mapChildEntities.end())
			return;

		// Get index of entity to be deleted
		size_t entityIndex = m_mapChildEntities.at(argName);

		// Swap entity to be deleted and the
		// entity at the back of the vector
		std::swap(m_children[entityIndex], m_children.back());

		// Update entity handle
		m_mapChildEntities.at(m_children[entityIndex]->getName()) = entityIndex;
		m_children[entityIndex]->setHandle(entityIndex);

		// Erase entity name and handle from map,
		// Pop entity from back of vector
		m_mapChildEntities.erase(argName);
		m_children.pop_back();
		*/
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