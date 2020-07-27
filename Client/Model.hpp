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

	public:
		Model();

		void loadFromFile(const std::string& argFilePath);
		void populateModelData(const aiScene* const argScene);
		void bindMesh(Helpers::Mesh* const argMesh);

		// Retrieve the dimensions of this model in local coordinates
		void getLocalExtents(glm::vec3& argMinExtents, glm::vec3& argMaxExtents) const;

		// Retrieves the collection of mesh loaded from the 3D model
		inline std::vector<std::unique_ptr<Helpers::Mesh>>& getMeshVector() { return m_meshes; }
	};
}

