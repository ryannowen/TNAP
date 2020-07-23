#include "Entity.hpp"

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
	}

	inline void Entity::addChild(const TNAP::Entity& argEntity)
	{
	}

	TNAP::Entity* Entity::findChild(const std::string& argName)
	{
		return nullptr;
	}

	TNAP::Entity* Entity::findChildRecursive(const std::string& argName)
	{
		return nullptr;
	}

	void Entity::destroySelf()
	{
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