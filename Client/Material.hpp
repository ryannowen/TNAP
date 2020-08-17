#pragma once

#include "ExternalLibraryHeaders.h"

#include <functional>

namespace TNAP {

	enum class EMaterialType
	{
		eUnlit,
		eUnlitTexture,
		ePBR,
		eCount
	};

	class Material
	{
	protected:
		friend class Renderer3D;

		std::string m_name{ "Unknown!" };

		size_t m_programHandle{ 0 };

		glm::vec4 m_colourTint{ 1 };

		size_t m_emissionTextureHandle{ 0 };

		glm::vec3 m_emissionColour{ 1 };
		float m_emissionIntensity{ 0 };
		
	public:
		Material();

		virtual void init();
		virtual void sendShaderData(const GLuint argProgram);
		inline virtual const EMaterialType getMaterialType() const { return EMaterialType::eUnlit; }

		virtual void saveData(std::ofstream& outputFile, const std::string& argShaderName);
		virtual void setData(const std::string& argData);

		inline void setColourTint(const glm::vec4& argColour) { m_colourTint = argColour; }

		void setEmissionTexture(const std::string& argFilePath);
		inline void setEmissionColour(const glm::vec3& argColour) { m_emissionColour = argColour; }
		inline void setEmissionIntesity(const float argIntensity) { m_emissionIntensity = argIntensity; }

		inline const size_t getProgramHandle() const { return m_programHandle; }

		inline const std::string& getName() const { return m_name; }

		inline const size_t getEmissionTextureHandle() const { return m_emissionTextureHandle; }
		inline const glm::vec4& getColourTint() const { return m_colourTint; }
		inline const glm::vec3& getEmissionColour() const { return m_emissionColour; }
		inline const float getEmissionIntensity() const { return m_emissionIntensity; }

#if USE_IMGUI
		virtual void imGuiRender();
#endif
	};
}