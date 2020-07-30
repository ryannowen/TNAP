#include "Light.hpp"

namespace TNAP {

	int Light::s_numberOfLights{ 0 };

	Light::Light() 
		: Entity()
	{
	}

	Light::~Light()
	{
	}

	void Light::update(const glm::mat4& parentTransform)
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