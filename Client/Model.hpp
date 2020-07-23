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

	};
}

