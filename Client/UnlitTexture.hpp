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

		virtual void sendShaderData(const GLuint argProgram) override;
		inline virtual const EMaterialType getMaterialType() const override { return EMaterialType::eUnlitTexture; }

		virtual void saveData(std::ofstream& outputFile, const std::string& argShaderName) override;
		virtual void setData(const std::string& argData) override;

		void setTexture(const ETextureType argTextureType, const std::string& argFilePath);
		const size_t getTextureHandle() const { return m_textureHandle; }

		inline const ETextureType getTextureType() const { return m_textureType; }

#if USE_IMGUI
		virtual void imGuiRender() override;
#endif

	};
}

