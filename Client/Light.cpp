#include "Light.hpp"

namespace TNAP {

	int Light::s_numberOfLights{ 0 };

	Light::Light()
	{
	}

	Light::~Light()
	{
	}

	void Light::update(const Transform& argTransform)
	{
	}

	void Light::sendNumberOfLights(const GLuint argProgram)
	{
	}

#if USE_IMGUI
	void Light::imGuiRenderProperties()
	{
	}
#endif

}