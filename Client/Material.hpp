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
	private:
		friend class Renderer3D;

		TNAP::EMaterialType m_materialType{ TNAP::EMaterialType::eUnlit };
		size_t m_programHandle{ 0 };

		glm::vec4 m_colourTint{ 0 };

		bool m_hasEmissionTexture{ false };
		size_t m_emissionTextureHandle{ 0 };

		glm::vec3 m_emissionColour{ 0 };
		float m_emissionIntesity{ 0 };

	public:
		Material();

		virtual void init();
		virtual void sendShaderData();


		void setEmissionTexture(const std::string& argFilePath);
		void setEmissionColour(const glm::vec3& argColour);
		void setEmissionIntesity(const float argIntensity);

		inline const EMaterialType getMaterialType() const { return m_materialType; }
		inline const size_t getProgramHandle() const { return m_programHandle; }

#if USE_IMGUI
		virtual void imGuiRender();
#endif
	};
}

