#pragma once
#include "Material.hpp"

#include <array>
#include <bitset>

#include "Renderer3D.hpp"

namespace TNAP {

	class PBR : public TNAP::Material
	{
	private:
		std::array<size_t, 6> m_textureHandles;
		std::bitset<6> m_usedTextures;

		float m_defaultMetallic{ 0 };
		float m_defualtRougness{ 0 };

	public:
		PBR();

		virtual void init() override final;
		virtual void sendShaderData() override final;

		void setTexture(const ETextureType argType, const std::string& argFilePath);
		inline void setMetallicDefaultValue(const float argMetallicValue) { m_defaultMetallic = argMetallicValue; }
		inline void setRoughnessDefaultValue(const float argRoughnessValue) { m_defualtRougness = argRoughnessValue; }

#if USE_IMGUI
		virtual void imGuiRender() override final;
#endif
	};
}
