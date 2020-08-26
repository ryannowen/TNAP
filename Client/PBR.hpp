#pragma once

#include "Material.hpp"

#include <array>
#include <bitset>

#include "Renderer3D.hpp"

namespace TNAP {

	class PBR : public TNAP::Material
	{
	private:
		std::array<ETextureType, static_cast<int>(TNAP::ETextureType::eCount) - 1> m_textureTypes;
		std::array<size_t, static_cast<int>(TNAP::ETextureType::eCount) - 1> m_textureHandles;

		float m_defaultMetallic{ 0 };
		float m_defaultRoughness{ 0 };

	public:
		PBR();

		virtual void init() override final;
		virtual void sendShaderData(const GLuint argProgram) const override final;
		inline virtual const EShaderType getShaderType() const override final { return EShaderType::ePBR; }

		virtual void saveData(std::ofstream& outputFile, const std::string& argShaderName) const;
		virtual void setData(const std::string& argData) override;

		void setTexture(const ETextureType argType, const std::string& argFilePath);
		inline void setMetallicDefaultValue(const float argMetallicValue) { m_defaultMetallic = argMetallicValue; }
		inline void setRoughnessDefaultValue(const float argRoughnessValue) { m_defaultRoughness = argRoughnessValue; }

		inline void setTextureTypes(const std::vector<ETextureType>& argTextureTypes);
		inline void setTextureHandles(const std::vector<size_t>& argTextureHandles);

#if USE_IMGUI
		virtual void imGuiRender() override final;
#endif
	};
}
