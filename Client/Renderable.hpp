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
		~Renderable();
		virtual void init() override;
		virtual void update(const glm::mat4& parentTransform) override;
#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
