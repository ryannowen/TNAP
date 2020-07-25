#include "Entity.hpp"
#include <memory>

namespace TNAP {

	Entity::Entity()
	{
	}

	Entity::~Entity()
	{
	}

	void Entity::init()
	{
	}

	void Entity::update(const TNAP::Transform& argTransform)
	{

		for (const std::shared_ptr<Entity>& entity : m_children)
		{
			// TODO: MULTIPLY current transform BY argTransform
			// TODO: Update children if enabled
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

	void Entity::sendMessage()
	{
	}

#if USE_IMGUI
	void Entity::imGuiRenderHierarchy()
	{
	}

	void Entity::imGuiRenderProperties()
	{
	}
#endif

}