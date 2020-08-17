#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Transform.hpp"
#include "SceneManager.hpp"
#include "Scene.hpp"

namespace TNAP {

	enum class EEntityType
	{
		eEntity,
		eRenderable,
		eLight,
		ePointLight,
		eSpotLight
	};

	class Message;

	class Entity
	{
		friend class Scene;
	protected:
		std::vector<size_t> m_children;

	private:
		size_t m_handle{ 0 };

		bool m_hasParent{ false };
		size_t m_parentHandle{ 0 };

		bool m_enabled{ true };
		std::string m_name{ "" };
		TNAP::Transform m_transform;

#if USE_IMGUI
		static Entity* s_selected;
		std::string m_namePlaceholder;
#endif

		void updateChildrenParentHandles();
		void deleteChildren();
		void removeChild(const size_t argHandle);
		void replaceChildHandle(const size_t argOld, const size_t argNew);

	public:
		Entity();
		~Entity();

		virtual void init();
		virtual void update(const glm::mat4& parentTransform);
		virtual void saveData(std::ofstream& outputFile);

		inline virtual const EEntityType getEntityType() const { return EEntityType::eEntity; }

		inline bool setName(const std::string& argName);
		inline const std::string& getName() const { return m_name; }

		bool updateNameInSceneMap(const std::string& argNewName);

		inline const TNAP::Transform& getLocalTransform() const { return m_transform; }
		inline TNAP::Transform& getTransform() { return m_transform; }

		inline void setHasParent(const bool argValue) { m_hasParent = argValue; }
		inline const bool getHasParent() const { return m_hasParent; }

		inline void setParentHandle(const size_t argHandle) { m_parentHandle = argHandle; }
		inline const size_t getParentHandle() const { return m_parentHandle; }

		void setChildren(const std::vector<size_t>& argChildrenHandles);

		template<class EntityType, typename... Args>
		inline EntityType* const addChild(const std::string& argName, const Args&... args);

		TNAP::Entity* const findChild(const size_t argHandle);
		TNAP::Entity* const findChild(const std::string& argName);
		TNAP::Entity* const findChildRecursive(const std::string& argName);

		void destroySelf();
		void destroyChild(const std::string& argName);
		void destroyChild(const size_t argHandle);

		inline void setHandle(const size_t argHandle) { m_handle = argHandle; }
		inline const size_t getHandle() const { return m_handle; }

		inline void setEnabled(const bool argValue) { m_enabled = argValue; }
		inline const bool getEnabled() const { return m_enabled; }

		void sendMessage(TNAP::Message* const argMessage);

#if USE_IMGUI
		void imGuiRenderHierarchy();
		virtual void imGuiRenderProperties();
		inline static Entity* const getSelected() { return s_selected; }
		inline static void setSelected(Entity* const argSelected) { s_selected = argSelected; }
#endif
	};

	template<class EntityType, typename... Args>
	inline EntityType* const Entity::addChild(const std::string& argName, const Args&... args)
	{
		static_assert(std::is_base_of<Entity, EntityType>::value, "addChild: Trying to create entity from non-entity type!");
		EntityType* const e = TNAP::getSceneManager()->getCurrentScene()->addEntity<EntityType>(true, argName, std::move(args)...);
		if (nullptr != e)
		{
			e->setParentHandle(m_handle);
			std::pair<bool, size_t> entityIndex { TNAP::getSceneManager()->getCurrentScene()->getEntityIndex(e->getName()) };
			if (entityIndex.first)
			{
				m_children.push_back(entityIndex.second);
			}
		}

		return e;
	}
}
