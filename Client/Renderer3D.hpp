#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "System.hpp"

#include "ImGuiInclude.hpp"
#include "IMGUILoadWindows.hpp"
#include "ExternalLibraryHeaders.h"
#include "ImageLoader.h"

#include "Model.hpp"
#include "Material.hpp"
#include "FrameBuffer.hpp"
#include "LoadModelMessage.hpp"

#include "Transform.hpp"
#include "STextureData.hpp"

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

	struct SProgram
	{
		GLuint m_program;
		EMaterialType m_type;
		std::string m_name;
		std::string m_vertexShader;
		std::string m_fragmentShader;
		SProgram(const GLuint argProgram, const EMaterialType argType, const std::string& argShaderName, const std::string& argVertexShaderPath, const std::string& argFragmentShaderPath)
		: m_program(argProgram), m_type(argType), m_name(argShaderName), m_vertexShader(argVertexShaderPath), m_fragmentShader(argFragmentShaderPath)
		{}
	};

	class Renderer3D : public TNAP::System
	{
	private:
		TNAP::FrameBuffer m_windowFrameBuffer;

		std::unordered_map<std::string, size_t> m_mapModels;
		std::vector<TNAP::Model> m_models;

		std::unordered_map<TNAP::ETextureType, std::unordered_map<std::string, size_t>> m_mapTextures;
		std::vector<std::vector<STextureData>> m_textures;

		std::unordered_map<std::string, size_t> m_mapMaterials;
		std::vector<std::unique_ptr<TNAP::Material>> m_materials;

		std::unordered_map<std::string, size_t> m_mapPrograms;
		std::vector<SProgram> m_programs;

		// Map<ModelHandle, Vector<Pair<Vector<ModelTransform>, Vector<MaterialHandles>>>>
		std::unordered_map<size_t, std::vector<std::pair<std::vector<glm::mat4>, std::vector<size_t>>>> m_batchRenders;

		std::vector<TNAP::Light*> m_nextLights;

		GLuint m_currentProgram{ 0 };
		GLuint batchRenderingBuffer{ 0 };
		
#if USE_IMGUI
		
		std::vector<std::unique_ptr<IMGUILoadBase>> m_loadWindows;
#endif

		const std::pair<bool, size_t> loadModel(const std::string& argFilePath, const std::string& argModelName = "");
		void loadShaders();
		const std::pair<bool, size_t> loadTexture(const TNAP::ETextureType argType, const std::string& argFilePath, const std::string& argTextureName = "");
		void loadMaterials(const std::string& argFilePath);
		const bool createShader(const EMaterialType argType, const std::string& argShaderName, const std::string& argVertexShaderPath, const std::string& argFragmentShaderPath);
		const bool createMaterial(const std::string& argMaterialName, const std::string& argShaderName, const bool argIncrementNameIfExisting = false);
		const bool createMaterial(const std::string& argMaterialName, const TNAP::EMaterialType argMaterialType, const bool argIncrementNameIfExisting = false);

		void submitModelMessage(TNAP::Message* const argMessage);
		void generateMaterialMessage(TNAP::Message* const argMessage);
		void glfwDropCallBackMessage(TNAP::Message* const argMessage);

		void render();

#if USE_IMGUI
		void imGuiRenderShelf();
		void imGuiRenderViewport();
#endif

	public:
		static const std::array<std::string, static_cast<int>(TNAP::ETextureType::eCount)> textureTypeNames;

		Renderer3D();
		~Renderer3D();

		virtual void init() override final;
		virtual void update() override final;
		virtual void sendMessage(TNAP::Message* const argMessage) override final;

#if USE_IMGUI
		virtual void imGuiRender() override final;
		void imGuiRenderMaterial(const size_t argMaterialHandle);
#endif

	};

	
}