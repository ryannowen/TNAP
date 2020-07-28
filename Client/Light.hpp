#pragma once

#include "Entity.hpp"
#include "ExternalLibraryHeaders.h"

namespace TNAP {

	class Light : public TNAP::Entity
	{
	private:
		glm::vec3 m_colour;
		float m_intensity;
		static int s_numberOfLights;

	public:
		Light();
		~Light();
		virtual void update(const glm::mat4& parentTransform) override;

		inline void setColour(const glm::vec3& argColour) { m_colour = argColour; }
		inline const glm::vec3& getColour() const { return m_colour; }

		inline void setIntensity(const float argIntensity) { m_intensity = argIntensity; }
		inline const float getIntensity() const { return m_intensity; }

		static void sendNumberOfLights(const GLuint argProgram);

#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
