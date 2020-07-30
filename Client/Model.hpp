#pragma once

#include <string>
#include <memory>>
#include <vector>
#include "Mesh.h"

namespace TNAP {

	class Model
	{
	private:
		std::vector<std::unique_ptr<Helpers::Mesh>> m_meshes;
		std::vector<size_t> m_uniqueMaterialIndices;
		std::vector<size_t> m_defaultMaterialHandles;
		
	public:
		Model();

		bool loadFromFile(const std::string& argFilePath);
		void populateModelData(const aiScene* const argScene);
		void bindMesh(Helpers::Mesh* const argMesh);
		inline const size_t getUniqueMaterialIndicesCount() const { return m_uniqueMaterialIndices.size(); }

		inline void addDefaultMaterialHandle(const size_t argHandle) { m_defaultMaterialHandles.emplace_back(argHandle); }
		inline const std::vector<size_t>& getDefaultMaterialHandles() const { return m_defaultMaterialHandles; }

		// Retrieve the dimensions of this model in local coordinates
		void getLocalExtents(glm::vec3& argMinExtents, glm::vec3& argMaxExtents) const;

		// Retrieves the collection of mesh loaded from the 3D model
		inline std::vector<std::unique_ptr<Helpers::Mesh>>& getMeshVector() { return m_meshes; }
		inline const std::vector<std::unique_ptr<Helpers::Mesh>>& getMeshVector() const { return m_meshes; }
	};
}

