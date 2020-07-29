#pragma once

#include "ExternalLibraryHeaders.h"

namespace TNAP {

	enum class EMaterialType
	{
		eUnlit,
		eUnlitTexture,
		ePBR
	};

	class Material
	{
	protected:
		friend class Renderer3D;

		std::string m_name{ "Unknown!" };

		TNAP::EMaterialType m_materialType{ TNAP::EMaterialType::eUnlit };
		size_t m_programHandle{ 0 };

		glm::vec4 m_colourTint{ 1 };

		size_t m_emissionTextureHandle{ 0 };

		glm::vec3 m_emissionColour{ 1 };
		float m_emissionIntensity{ 0 };

	public:
		Material();

		virtual void init();
		virtual void sendShaderData(const GLuint argProgram);

		inline void setColourTint(const glm::vec4& argColour) { m_colourTint = argColour; }

		void setEmissionTexture(const std::string& argFilePath);
		inline void setEmissionColour(const glm::vec3& argColour) { m_emissionColour = argColour; }
		inline void setEmissionIntesity(const float argIntensity) { m_emissionIntensity = argIntensity; }

		inline const EMaterialType getMaterialType() const { return m_materialType; }
		inline const size_t getProgramHandle() const { return m_programHandle; }

		inline const std::string& getName() const { return m_name; }

#if USE_IMGUI
		virtual void imGuiRender();
#endif
	};
}

