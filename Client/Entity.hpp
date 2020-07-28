#pragma once

#include "Transform.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace TNAP {

	class Message;

	class Entity
	{
	protected:
		std::vector<std::shared_ptr<TNAP::Entity>> m_children;

	private:
		size_t m_handle{ 0 };
		bool m_enabled{ true };
		std::string m_name = "";
		TNAP::Transform m_transform;

		std::unordered_map<std::string, size_t> m_mapChildEntities;

#if USE_IMGUI
		static Entity* s_selected;
		static int s_treeIndex;
#endif

	public:
		Entity();
		~Entity();

		virtual void init();
		virtual void update(const glm::mat4& parentTransform);

		inline void setName(const std::string& argName) { m_name = argName; }
		inline const std::string& getName() const { return m_name; }

		inline const TNAP::Transform& getLocalTransform() const { return m_transform; }
		inline TNAP::Transform& getTransform() { return m_transform; }

		template<class EntityType, typename... Args>
		inline EntityType* const addChild(const std::string& argName, const Args&... args);

		TNAP::Entity* const findChild(const size_t argHandle);
		TNAP::Entity* const findChild(const std::string& argName);
		TNAP::Entity* const findChildRecursive(const std::string& argName);
		void destroySelf();

		inline void setHandle(const size_t argHandle) { m_handle = argHandle; }
		inline const size_t getHandle() const { return m_handle; }

		inline void setEnabled(const bool argValue) { m_enabled = argValue; }
		inline const bool getEnabled() const { return m_enabled; }

		void sendMessage(TNAP::Message* const argMessage);

#if USE_IMGUI
		void imGuiRenderHierarchy();
		virtual void imGuiRenderProperties();
		inline static Entity* const getSelected() { return s_selected; }
		inline static const int getTreeIndex() { return s_treeIndex; }
		inline static void setSelected(Entity* const argSelected) { s_selected = argSelected; }
		inline static void setTreeIndex(const int argIndex) { s_treeIndex = argIndex; }
#endif
	};

	template<class EntityType, typename... Args>
	inline EntityType* const Entity::addChild(const std::string& argName, const Args&... args)
	{
		if (m_mapChildEntities.insert({ argName, m_children.size() }).second)
		{
			m_children.emplace_back(std::make_shared<EntityType>(args...));
			m_children.back()->setName(argName);
			return static_cast<EntityType*>(m_children.back().get());
		}

		return nullptr;
	}

}
