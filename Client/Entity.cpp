#include "Entity.hpp"
#include "ImGuiInclude.hpp"
#include <memory>

namespace TNAP {

#if USE_IMGUI
	Entity* Entity::s_selected{ nullptr };
	int Entity::s_treeIndex{ 0 };
#endif

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
		for (const std::shared_ptr<Entity>& entity : m_children)
		{
			if (entity->getEnabled())
				entity->update(parentTransform * getTransform().getMatrix());
		}
	}

	TNAP::Entity* const Entity::findChild(const size_t argHandle)
	{
		if (argHandle >= 0 && argHandle < m_children.size())
			return m_children[argHandle].get();

		return nullptr;
	}

	TNAP::Entity* const Entity::findChild(const std::string& argName)
	{
		if (m_mapChildEntities.find(argName) == m_mapChildEntities.end())
			return nullptr;

		return m_children[m_mapChildEntities.at(argName)].get();
	}

	TNAP::Entity* const Entity::findChildRecursive(const std::string& argName)
	{
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
	}

	void Entity::destroySelf()
	{
		// TODO: Send message to scene / parent entity
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
				m_children[i]->imGuiRenderHierarchy();
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
			ImGui::DragFloat3("Translation", &getTransform().getTranslation().x);
			ImGui::DragFloat3("Rotation", &getTransform().getRotation().x);
			ImGui::DragFloat3("Scale", &getTransform().getScale().x);
		}
	}
#endif

}