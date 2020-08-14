#pragma once
#include "Material.hpp"

#include <array>
#include <bitset>

#include "Renderer3D.hpp"

namespace TNAP {

	class PBR : public TNAP::Material
	{
	private:
		std::array<size_t, static_cast<int>(TNAP::ETextureType::eCount)> m_textureHandles;

		float m_defaultMetallic{ 0 };
		float m_defaultRoughness{ 0 };

	public:
		PBR();

		virtual void init() override final;
		virtual void sendShaderData(const GLuint argProgram) override final;
		inline virtual const EMaterialType getMaterialType() const override final { return EMaterialType::ePBR; }

		void setTexture(const ETextureType argType, const std::string& argFilePath);
		inline void setMetallicDefaultValue(const float argMetallicValue) { m_defaultMetallic = argMetallicValue; }
		inline void setRoughnessDefaultValue(const float argRoughnessValue) { m_defaultRoughness = argRoughnessValue; }

#if USE_IMGUI
		virtual void imGuiRender() override final;
#endif
	};
}
