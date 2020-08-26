#pragma once

#include "ExternalLibraryHeaders.h"

#include <functional>

namespace TNAP {

	enum class EShaderType
	{
		eUnlit,
		eUnlitTexture,
		ePBR,
		eCount,
		eEarlyDepthTest
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
		
		bool m_useTransparency{ false };
		bool m_doubleSided{ false };

	public:
		Material();

		virtual void init();
		virtual void sendShaderData(const GLuint argProgram) const;
		inline virtual const EShaderType getShaderType() const { return EShaderType::eUnlit; }

		virtual void saveData(std::ofstream& outputFile, const std::string& argShaderName) const;
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

		inline void setUseTransparency(const bool argUseTransparency) { m_useTransparency = argUseTransparency; }
		inline const bool getUseTransparency() const { return m_useTransparency; }

		inline void setDoubleSided(const bool argDoubleSided) { m_doubleSided = argDoubleSided; }
		inline const bool getDoubleSided() const { return m_doubleSided; }

#if USE_IMGUI
		virtual void imGuiRender();
#endif
	};
}