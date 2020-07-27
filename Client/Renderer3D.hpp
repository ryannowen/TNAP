#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "System.hpp"

#include "ExternalLibraryHeaders.h"
#include "ImageLoader.h"

#include "Model.hpp"
#include "Material.hpp"
#include "FrameBuffer.hpp"

#include "Transform.hpp"

namespace TNAP {

	class Renderable;
	class Light;

	enum class ETextureType
	{
		eAlbedo,
		eNormal,
		eMetallic,
		eRoughness,
		eAO,
		eEmission,
		eCount
	};

	class Renderer3D : public TNAP::System
	{
	private:
		static GLFWwindow* s_window;
		glm::vec2 m_windowSize{ 1600, 900 };

		TNAP::FrameBuffer m_windowFrameBuffer;

		std::unordered_map<std::string, size_t> m_mapModels;
		std::vector<TNAP::Model> m_models;

		std::unordered_map<TNAP::ETextureType, std::unordered_map<std::string, size_t>> m_mapTextures;
		std::vector<std::vector<std::pair<std::unique_ptr<Helpers::ImageLoader>, GLuint>>> m_textures;

		std::unordered_map<std::string, size_t> m_mapMaterials;
		std::vector<std::unique_ptr<TNAP::Material>> m_materials;

		std::unordered_map<std::string, size_t> m_mapPrograms;
		std::vector<GLuint> m_programs;

		// Map<ModelHandle, Vector<Pair<Vector<ModelTransform>, Vector<MaterialHandles>>>>
		std::unordered_map<size_t, std::vector<std::pair<std::vector<TNAP::Transform>, std::vector<size_t>>>> m_batchRenders;

		std::vector<TNAP::Renderable*> m_nextRenders;
		std::vector<TNAP::Light*> m_nextLights;

		void CreateProgram(const std::string& argVertexFilePath, const std::string& argFragmentFilePath);

		GLuint m_currentProgram{ 0 };

	public:
		Renderer3D();
		~Renderer3D();

		virtual void init() override final;
		virtual void update() override final;
		virtual void sendMessage(TNAP::Message* const argMessage) override final;

		void loadModel(const std::string& argFilePath);
		void loadTexture(const TNAP::ETextureType argType, const std::string& argFilePath);
		void loadMaterials(const std::string& argFilePath);

		void render();

		inline static GLFWwindow* getWindow() { return s_window; }

#if USE_IMGUI
		virtual void imGuiRender() override final;
		void imGuiRenderMaterial(const size_t argMaterialHandle);
#endif

	};
}