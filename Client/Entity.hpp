#pragma once

#include "Transform.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace TNAP {

	class Entity
	{
	private:
		size_t m_handle{ 0 };
		bool m_enabled{ true };
		std::string m_name = "";
		TNAP::Transform m_transform;
		
		std::unordered_map<std::string, size_t> m_mapChildEntities;
		std::vector<std::shared_ptr<TNAP::Entity>> m_children;

	public:
		Entity();
		~Entity();

		virtual void init();
		virtual void update(const TNAP::Transform& argTransform);

		inline void setName(const std::string& argName) { m_name = argName; }
		inline const std::string& getName() const { return m_name; }

		inline const TNAP::Transform& getLocalTransform() const { return m_transform; }
		inline TNAP::Transform& getTranslation() { return m_transform; }

		template<class EntityType, typename... Args>
		inline EntityType* const addEntity(const std::string& argName, const Args&... args);

		TNAP::Entity* const findChild(const size_t argHandle);
		TNAP::Entity* const findChild(const std::string& argName);
		TNAP::Entity* const findChildRecursive(const std::string& argName);
		void destroySelf();

		inline void setHandle(const size_t argHandle) { m_handle = argHandle; }
		inline const size_t getHandle() const { return m_handle; }

		inline void setEnabled(const bool argValue) { m_enabled = argValue; }
		inline const bool getEnabled() const { return m_enabled; }

		void sendMessage();

#if USE_IMGUI
		void imGuiRenderHierarchy();
		virtual void imGuiRenderProperties();
#endif
	};

	template<class EntityType, typename... Args>
	inline EntityType* const Entity::addEntity(const std::string& argName, const Args&... args)
	{

		if (m_mapChildEntities.find(argName) == m_mapChildEntities.end())
		{
			m_children.emplace_back(std::make_shared<EntityType>(args...));
			m_children.back()->setName(argName);
			m_mapChildEntities.insert({ argName, m_children.size() - 1 });
			return m_children.back();
		}

		return nullptr;
	}

}
