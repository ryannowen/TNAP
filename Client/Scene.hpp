#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace TNAP {
	
	class Entity;

	class Scene
	{
	private:
		std::unordered_map<std::string, size_t> m_mapEntities;
		std::vector<std::shared_ptr<TNAP::Entity>> m_entities;

	public:
		Scene();
		~Scene();

		void init();
		void update();
		void addEntity(const TNAP::Entity& argEntity);
		void findEntity(const std::string& argName);
		void destroyEntity(const std::string& argName);
		void destroyEntity(const size_t argHandle);
		void sendMessage();
	};

}
