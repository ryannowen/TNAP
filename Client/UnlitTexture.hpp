#pragma once

#include "Material.hpp"

#include "Renderer3D.hpp"

namespace TNAP {
	class UnlitTexture : public TNAP::Material
	{
	private:
		ETextureType m_textureType{ ETextureType::eAlbedo };
		size_t m_textureHandle{ 0 };

	public:
		UnlitTexture();

		virtual void sendShaderData(const GLuint argProgram) const override;
		inline virtual const EShaderType getShaderType() const override { return EShaderType::eUnlitTexture; }

		virtual void saveData(std::ofstream& outputFile, const std::string& argShaderName) const override;
		virtual void setData(const std::string& argData) override;

		void setTexture(const ETextureType argTextureType, const std::string& argFilePath);
		const size_t getTextureHandle() const { return m_textureHandle; }

		inline const ETextureType getTextureType() const { return m_textureType; }

#if USE_IMGUI
		virtual void imGuiRender() override;
#endif

	};
}

