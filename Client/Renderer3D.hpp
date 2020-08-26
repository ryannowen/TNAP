#pragma once

#include "System.hpp"

#include <unordered_map>
#include <vector>
#include <memory>

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
	struct SLightData;

	enum class ERenderPass
	{
		eEarlyDepthTest
		, eOpaqueRender
		, eTransparentRender
		//eLighting
		, eCount
	};

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

	struct SProgramData
	{
		GLuint m_program;
		EShaderType m_type;
		std::string m_name;
		bool m_useLighting{ false };
		std::string m_vertexShader;
		std::string m_fragmentShader;
		SProgramData(const GLuint argProgram, const EShaderType argType, const std::string& argShaderName, const bool argUseLighting, const std::string& argVertexShaderPath, const std::string& argFragmentShaderPath)
		: m_program(argProgram), m_type(argType), m_name(argShaderName), m_useLighting(argUseLighting), m_vertexShader(argVertexShaderPath), m_fragmentShader(argFragmentShaderPath)
		{}
	};

	class Renderer3D : public TNAP::System
	{
	private:
		TNAP::FrameBuffer m_earlyDepthTestFrameBuffer;
		TNAP::FrameBuffer m_geometryFrameBuffer;

		std::unordered_map<std::string, size_t> m_mapModels;
		std::vector<TNAP::Model> m_models;

		std::unordered_map<TNAP::ETextureType, std::unordered_map<std::string, size_t>> m_mapTextures;
		std::vector<std::vector<TNAP::STextureData>> m_textures;

		std::unordered_map<std::string, size_t> m_mapMaterials;
		std::vector<std::unique_ptr<TNAP::Material>> m_materials;

		std::unordered_map<std::string, size_t> m_mapPrograms;
		std::vector<TNAP::SProgramData> m_programs;

		// Map<ModelHandle, Vector<Pair<Vector<ModelTransform>, Vector<MaterialHandles>>>>
		//std::unordered_map<size_t, std::vector<std::pair<std::vector<glm::mat4>, std::vector<size_t>>>> m_batchRenders;

		//map<size_t, map<size_t, pair<map<size_t, pair<vector<mat4>, size_t>>, map<size_t, pair<vector<mat4>, size_t>>>>>

		// Material Handle
		using MeshData = size_t;
		// Transforms and Mesh indices
		using MeshBatch = std::pair<std::vector<glm::mat4>, std::unordered_map<size_t, std::vector<MeshData>>>;
		// Model handle
		using ModelBatch = std::unordered_map<size_t, std::pair<MeshBatch, MeshBatch>>;
		// Program handle
		using ProgramBatch = std::unordered_map<size_t, ModelBatch>;


		ProgramBatch m_batchRenders;


		std::vector<std::unique_ptr<TNAP::SLightData>> m_nextLights;

		GLuint m_currentProgram{ 0 };
		GLuint m_batchRenderingBuffer{ 0 };
		
#if USE_IMGUI
		bool m_viewportSelected{ false };
		std::vector<std::unique_ptr<TNAP::IMGUILoadBase>> m_loadWindows;
#endif

		const std::pair<bool, size_t> loadModel(const std::string& argFilePath, const std::string& argModelName = "");
		void loadShaders();
		const std::pair<bool, size_t> loadTexture(const TNAP::ETextureType argType, const std::string& argFilePath, const std::string& argTextureName = "");
		void loadMaterials(const std::string& argFilePath);
		const bool createProgram(const TNAP::EShaderType argType, const std::string& argShaderName, const bool argUseLighting, const std::string& argVertexShaderPath, const std::string& argFragmentShaderPath);
		const bool createShader(const GLuint argProgram, const GLenum argShaderType, const std::string& argShaderPath);
		const bool createMaterial(const std::string& argMaterialName, const std::string& argShaderName, const bool argIncrementNameIfExisting = false);
		const bool createMaterial(const std::string& argMaterialName, const TNAP::EShaderType argShaderType, const bool argIncrementNameIfExisting = false);

		const std::pair<bool, size_t> getProgramHandle(const TNAP::EShaderType argType);

		void saveMaterials();

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