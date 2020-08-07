#include "Entity.hpp"
#include "ImGuiInclude.hpp"
#include <memory>
#include <fstream>

namespace TNAP {

#if USE_IMGUI
	Entity* Entity::s_selected{ nullptr };
	int Entity::s_treeIndex{ 0 };
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
		outputFile << m_enabled << ",";
		outputFile << m_hasParent << ",";
		outputFile << m_parentHandle << ",";
		outputFile << m_transform.getTranslation().x << " " << m_transform.getTranslation().y << " " << m_transform.getTranslation().z << "|";
		outputFile << m_transform.getRotation().x << " " << m_transform.getRotation().y << " " << m_transform.getRotation().z << "|";
		outputFile << m_transform.getScale().x << " " << m_transform.getScale().y << " " << m_transform.getScale().z;
	}

	TNAP::Entity* const Entity::findChild(const size_t argHandle)
	{
		return TNAP::getSceneManager()->getCurrentScene()->findEntity(argHandle);
	}

	TNAP::Entity* const Entity::findChild(const std::string& argName)
	{
		return TNAP::getSceneManager()->getCurrentScene()->findEntity(argName);
	}

	TNAP::Entity* const Entity::findChildRecursive(const std::string& argName)
	{
		/*
		const auto& it(m_mapChildEntities.find(argName));
		if (it == m_mapChildEntities.end())
		{
			if (0 == m_children.size())
				return nullptr;

			for (const std::shared_ptr<Entity>& child : m_children)
			{
				Entity* entity = child->findChildRecursive(argName);
				if (nullptr != entity)
					return entity;
			}
		}

		return m_children[it->second].get();
		*/
		return nullptr;
	}

	void Entity::destroySelf()
	{
		// TODO: Send message to scene / parent entity
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
		/*
		// Invalid entity handle
		if (argHandle > m_children.size() || argHandle < 0)
			return;

		// Swap entity to be deleted and the
		// entity at the back of the vector
		std::swap(m_children[argHandle], m_children.back());

		// Update entity handle
		m_mapChildEntities.at(m_children[argHandle]->getName()) = argHandle;
		m_children[argHandle]->setHandle(argHandle);

		// Erase entity name and handle from map,
		// Pop entity from back of vector
		m_mapChildEntities.erase(m_children.back()->getName());
		m_children.pop_back();
		*/
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

		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)s_treeIndex, node_flags, m_name.c_str());

		if (ImGui::IsItemClicked())
			s_selected = this;

		s_treeIndex++;

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
		ImGui::Text(("Name: " + getName()).c_str());
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