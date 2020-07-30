#include "PointLight.hpp"

namespace TNAP {

	PointLight::PointLight() 
		: Light()
	{
	}

	PointLight::~PointLight()
	{
	}

	void PointLight::update(const glm::mat4& parentTransform)
	{
	}

#if USE_IMGUI
	void PointLight::imGuiRenderProperties()
	{
	}
#endif

}
