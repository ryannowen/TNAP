#include "Renderable.hpp"

namespace TNAP {

	Renderable::Renderable()
	{
	}
	Renderable::~Renderable()
	{
	}

	void TNAP::Renderable::init()
	{
	}

	void TNAP::Renderable::update(const TNAP::Transform& argTransform)
	{
	}

#if USE_IMGUI
	void TNAP::Renderable::imGuiRenderProperties()
	{
	}
#endif

}
