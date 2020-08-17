#pragma once

#include "Entity.hpp"

#include <vector>

namespace TNAP {

	class Renderable : public TNAP::Entity
	{
	private:
		size_t m_modelHandle{ 0 };
		std::vector<size_t> m_materialHandles{ 0 };

	public:
		Renderable();
		Renderable(const std::string& argFilepath);
		~Renderable();

		virtual void init() override;
		virtual void update(const glm::mat4& parentTransform) override;
		virtual void saveData(std::ofstream& outputFile) override;
		inline virtual const EEntityType getEntityType() const override { return EEntityType::eRenderable; }

		void loadModel(const std::string& argFilepath);
		void setMaterialHandles(const std::vector<size_t>& argMaterialHandles);

#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
