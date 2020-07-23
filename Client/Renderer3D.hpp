#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "System.hpp"

#include "ExternalLibraryHeaders.h"
#include "ImageLoader.h"

#include "Model.hpp"
#include "Material.hpp"

class Renderable;
class Light;

namespace TNAP {
	enum class ETextureType
	{
		eAlbedo,
		eNormal,
		eMetallic,
		eRoughness,
		eAO,
		eEmission
	};

	class Renderer3D : public TNAP::System
	{
	private:
		static GLFWwindow* s_window;

		std::unordered_map<std::string, size_t> m_mapModels;
		std::vector<TNAP::Model> m_models;

		std::unordered_map<TNAP::ETextureType, std::unordered_map<std::string, size_t>> m_mapTextures;
		std::vector<std::vector<std::pair<std::unique_ptr<Helpers::ImageLoader>, GLuint>>> m_textures;

		std::unordered_map<std::string, size_t> m_mapMaterials;
		std::vector<std::unique_ptr<Material>> m_materials;

		std::unordered_map<std::string, size_t> m_mapPrograms;
		std::vector<GLuint> m_programs;

		std::vector<Renderable*> m_nextRenders;
		std::vector<Light*> m_nextLights;

		void CreateProgram(const std::string& argVertexFilePath, const std::string& argFragmentFilePath);

		// TEMP
		GLuint m_program{ 0 };

	public:
		Renderer3D();
		~Renderer3D();

		virtual void init() override final;
		virtual void update() override final;
		virtual void sendMessage() override final;

		void loadModel(const std::string& argFilePath);
		void loadTexture(const std::string& argFilePath);
		void loadMaterials(const std::string& argFilePath);

		void render();

		inline static GLFWwindow* getWindow() { return s_window; }

#if USE_IMGUI
		virtual void imGuiRender() override final;
		void imGuiRenderMaterial();
#endif

	};
}
