#pragma once

#include "Entity.hpp"
#include <vector>

namespace TNAP {

	class Renderable : public TNAP::Entity
	{
	private:
		size_t m_modelHandle;
		std::vector<size_t> m_materialHandles;

	public:
		Renderable();
		~Renderable();
		virtual void init() override;
		virtual void update(const TNAP::Transform& argTransform) override;
#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
