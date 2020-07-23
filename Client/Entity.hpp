#pragma once

#include "Transform.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace TNAP {

	class Entity
	{
	private:
		bool m_enabled{ true };
		std::string m_name = "";
		TNAP::Transform m_transform;
		
		std::unordered_map<std::string, size_t> m_childEntities;
		std::vector<std::shared_ptr<TNAP::Entity>> m_children;

	public:
		Entity();
		~Entity();

		virtual void init();
		virtual void update(const TNAP::Transform& argTransform);

		inline const TNAP::Transform& getLocalTransform() const { return m_transform; }
		inline TNAP::Transform& getTranslation() { return m_transform; }

		inline void addChild(const TNAP::Entity& argEntity);
		TNAP::Entity* findChild(const std::string& argName);
		TNAP::Entity* findChildRecursive(const std::string& argName);
		void destroySelf();

		inline void setEnabled(const bool argValue) { m_enabled = argValue; }
		inline const bool getEnabled() const { return m_enabled; }

		void sendMessage();

#if USE_IMGUI
		void imGuiRenderHierarchy();
		virtual void imGuiRenderProperties();
#endif
	};

}
