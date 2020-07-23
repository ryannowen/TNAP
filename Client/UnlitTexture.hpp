#pragma once
#include "Material.hpp"

#include "Renderer3D.hpp"

namespace TNAP {
	class UnlitTexture : public TNAP::Material
	{
	private:
		size_t m_textureHandle{ 0 };

	public:
		UnlitTexture();

		virtual void sendShaderData() override;
		
		void setTexture(const ETextureType argTextureType, const std::string& argFilePath);
		const size_t getTextureHandle() const { return m_textureHandle; }

#if USE_IMGUI
		virtual void imGuiRender() override;
#endif

	};
}

