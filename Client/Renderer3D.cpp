#include "Renderer3D.hpp"

#include <array>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include "ExternalLibraryHeaders.h"
#include "Engine.hpp"
#include "Utilities.hpp"
#include "Application.hpp"

#include "Mesh.h"
#include "Simulation.h"
#include "Light.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

#include "GLFWCallBacks.hpp"
#include "GetTextureMessage.hpp"
#include "LoadTextureMessage.hpp"
#include "SubmitModelMessage.hpp"
#include "SubmitLightDataMessage.hpp"
#include "GetMaterialMessage.hpp"
#include "LogMessage.hpp"
#include "GenerateMaterialMessage.hpp"
#include "GetModelInfoMessage.hpp"
#include "GetMaterialHandleMessage.hpp"

#include "UnlitTexture.hpp"
#include "PBR.hpp"

namespace TNAP {

	const std::array<std::string, static_cast<int>(TNAP::ETextureType::eCount)> Renderer3D::textureTypeNames { "Albedo", "Normal", "Metallic", "Roughness", "AO", "Emission" };
	
	const std::pair<bool, size_t> Renderer3D::loadModel(const std::string& argFilePath, const std::string& argModelName)
	{
		LogMessage logMessage("");
		if (argFilePath.empty())
		{
			logMessage.m_message = "[Model] Filepath is empty, returning handle 0";
			logMessage.m_logType = LogMessage::ELogType::eWarning;
			TNAP::getEngine()->sendMessage(&logMessage);

			return std::pair<bool, size_t>(false, 0);
		}

		std::string filePath{ argFilePath };

		// Replaces File Path differences to forward slash
		static std::vector<std::string> replacements{ "\\\\", "\\", "//" };
		filePath = TNAP::replaceSubStrings(replacements, filePath, "/");

		std::string modelName{ argModelName };
		
		// If no name is given replaces filepath to only have filename
		if (modelName.empty())
		{
			modelName = filePath;

			size_t offset = modelName.find_last_of('/');

			if (offset != std::string::npos)
				modelName.replace(0, offset + 1, "");
		}

		const auto& modelFind{ m_mapModels.find(modelName) };
		if (modelFind != m_mapModels.end())
		{
			/*logMessage.m_message = "[Model] " + filePath + " is already loaded";
			logMessage.m_logType = LogMessage::ELogType::eInfo;
			TNAP::getEngine()->sendMessage(&logMessage);*/
			return  std::pair<bool, size_t>(true, modelFind->second);
		}

		m_models.emplace_back(TNAP::Model());

		if (!m_models.back().loadFromFile("Data/Models/" + filePath, m_batchRenderingBuffer))
		{
			logMessage.m_message = "[Model] " + filePath + " could not load, returning handle 0";
			logMessage.m_logType = LogMessage::ELogType::eWarning;
			TNAP::getEngine()->sendMessage(&logMessage);

			// If file fails to load, return first model
			return std::pair<bool, size_t>(false, 0);
		}
		m_models.back().setFilepath(filePath);

		m_mapModels.insert({ modelName, m_models.size() - 1 });

		// Setup materials
		for (int i = 0; i < m_models.back().getUniqueMaterialIndicesCount(); i++)
		{
			std::string name = modelName + "_" + std::to_string(i);
			const auto& it{ m_mapMaterials.find(name) };
			if (it != m_mapMaterials.end())
			{
				// material already Created
				m_models.back().addDefaultMaterialHandle(it->second);
				continue;
			}

			createMaterial(name, EMaterialType::eUnlitTexture, false);

			const auto& mapIndex{ m_mapMaterials.find(name) };
			
			if(mapIndex != m_mapMaterials.end())
				m_models.back().addDefaultMaterialHandle(m_mapMaterials.size() - 1);
		}

		logMessage.m_message = "[Model] " + filePath + " loaded successfully";
		logMessage.m_logType = LogMessage::ELogType::eSuccess;
		TNAP::getEngine()->sendMessage(&logMessage);

		return std::pair<bool, size_t>(true, m_models.size() - 1);
	}

	void Renderer3D::loadShaders()
	{
		std::ifstream savedShaders;

		savedShaders.open("Data/SaveLoad/Shaders.csv");

		std::string name;
		std::string type;
		std::string useLighting;
		std::string vert;
		std::string frag;
		
		while (savedShaders.good())
		{
			std::getline(savedShaders, name, ',');
			std::getline(savedShaders, type, ',');
			std::getline(savedShaders, useLighting, ',');
			std::getline(savedShaders, vert, ',');
			std::getline(savedShaders, frag, '\n');
			createShader(static_cast<EMaterialType>(std::stoi(type)), name, std::stoi(useLighting), vert, frag);
		}

		savedShaders.close();
	}

	const std::pair<bool, size_t> Renderer3D::loadTexture(const TNAP::ETextureType argType, const std::string& argFilePath, const std::string& argTextureName)
	{
		if (argFilePath.empty())
			return std::pair<bool, size_t>(false, 0);

		if (m_mapTextures.find(argType) == m_mapTextures.end())
			m_mapTextures.insert({ argType, std::unordered_map<std::string, size_t>() });

		std::string filePath{ argFilePath };

		// Replaces File Path differences to forward slash
		static std::vector<std::string> replacements{ "\\\\", "\\", "//" };
		filePath = TNAP::replaceSubStrings(replacements, filePath, "/");

		std::string textureName{ argTextureName };

		// If no name is given replaces filepath to only have filename
		if (textureName.empty())
		{
			textureName = filePath;
			size_t offset = textureName.find_last_of('/');

			if (offset != std::string::npos)
				textureName.replace(0, offset + 1, "");
		}

		LogMessage logMessage("");
		if (m_mapTextures.at(argType).find(textureName) != m_mapTextures.at(argType).end())
		{
			// Log Texture Already Loaded
			logMessage.m_message = "[Texture] " + filePath + " is already loaded";
			logMessage.m_logType = LogMessage::ELogType::eInfo;
			TNAP::getEngine()->sendMessage(&logMessage);
			size_t handle = m_mapTextures.at(argType).find(textureName)->second;
			return std::pair<bool, size_t>(true, handle);
		}

		std::unique_ptr<Helpers::ImageLoader> texture{ std::make_unique<Helpers::ImageLoader>() };

		if (!texture->Load("Data/Textures/" + filePath))
		{
			// Log Missing texture
			logMessage.m_message = "[Texture] " + filePath + " could not load";
			logMessage.m_logType = LogMessage::ELogType::eWarning;
			TNAP::getEngine()->sendMessage(&logMessage);
			return std::pair<bool, size_t>(false, 0);
		}

		// Log Loaded Texture
		logMessage.m_message = "[Texture] " + filePath + " loaded successfully";
		logMessage.m_logType = LogMessage::ELogType::eSuccess;
		TNAP::getEngine()->sendMessage(&logMessage);

		GLuint textureRef;

		/// Generates texture
		glGenTextures(1, &textureRef);
		glBindTexture(GL_TEXTURE_2D, textureRef);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->Width(), texture->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->GetData());
		glGenerateMipmap(GL_TEXTURE_2D);

		m_textures.at(static_cast<int>(argType)).emplace_back(STextureData(std::move(texture), textureRef, filePath));
		m_mapTextures.at(argType).insert({ textureName, m_textures.at(static_cast<int>(argType)).size() - 1 });

		return std::pair<bool, size_t>(true, m_textures.at(static_cast<int>(argType)).size() - 1);
	}

	void Renderer3D::loadMaterials(const std::string& argFilePath)
	{
		if (argFilePath.empty())
			return;

		createMaterial("DefaultMaterial", "Unlit");

		std::ifstream savedMaterials;

		savedMaterials.open("Data/SaveLoad/" + argFilePath);

		std::string material;

		while (std::getline(savedMaterials, material))
		{
			std::vector<std::string> materialInfo = TNAP::stringToStrings(material, ",", 3);
			if (createMaterial(materialInfo.at(0), materialInfo.at(1)))
				m_materials.at(m_mapMaterials.at(materialInfo.at(0)))->setData(materialInfo.at(2));
		}

		savedMaterials.close();
	}

	const bool Renderer3D::createShader(const EMaterialType argType, const std::string& argShaderName, const bool argUseLighting, const std::string& argVertexShaderPath, const std::string& argFragmentShaderPath)
	{
		if (argShaderName.empty() || argVertexShaderPath.empty() || argFragmentShaderPath.empty())
			return false;

		// Check if Shader is already created
		if (m_mapPrograms.find(argShaderName) != m_mapPrograms.end())
			return false;

		const GLuint program = glCreateProgram();

		// Load and create vertex and fragment shaders
		const GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, argVertexShaderPath) };
		const GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, argFragmentShaderPath) };

		if ((0 == vertex_shader) || (0 == fragment_shader))
		{
			glDeleteProgram(program);
			return false;
		}

		// Attach the vertex shader & fragment shader to this program (copies them)
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		// Done with the originals of these as we have made copies
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		// Link the shaders, checking for errors
		if (!Helpers::LinkProgramShaders(program))
			return false;

		SProgram newProgram(program, argType, argShaderName, argUseLighting, argVertexShaderPath, argFragmentShaderPath);

		m_programs.emplace_back(newProgram);
		m_mapPrograms.insert({ argShaderName, m_programs.size() - 1 });

		return true;
	}

	const bool Renderer3D::createMaterial(const std::string& argMaterialName, const std::string& argShaderName, const bool argIncrementNameIfExisting)
	{
		if (argMaterialName.empty() || argShaderName.empty())
			return false;

		if (m_mapPrograms.find(argShaderName) == m_mapPrograms.end())
			return false;

		std::string materialName{ argMaterialName };

		// material already Created 
		if (m_mapMaterials.find(materialName) != m_mapMaterials.end())
		{
			if (argIncrementNameIfExisting)
			{
				// Removes _IDNum from end of name
				size_t matNameIndex{ materialName.find_last_of("_") };

				if (std::string::npos == matNameIndex)
					matNameIndex = materialName.size();

				materialName.erase(matNameIndex);
				matNameIndex = 0;

				while (m_mapMaterials.find(materialName + "_" + std::to_string(matNameIndex)) != m_mapMaterials.end())
					matNameIndex++;

				materialName += "_" + std::to_string(matNameIndex);
			}
			else
				return true;
		}

		switch (m_programs.at(m_mapPrograms.at(argShaderName)).m_type)
		{
		case EMaterialType::eUnlitTexture:
		{
			m_materials.emplace_back(std::make_unique<UnlitTexture>());
		}
		break;

		case EMaterialType::ePBR:
		{
			m_materials.emplace_back(std::make_unique<PBR>());
		}
		break;

		default:
		case EMaterialType::eUnlit:
		{
			m_materials.emplace_back(std::make_unique<Material>());
		}
		break;
		}

		m_mapMaterials.insert({ materialName, m_materials.size() - 1 });

		m_materials.back()->m_programHandle = m_mapPrograms.at(argShaderName);
		m_materials.back()->m_name = materialName;
		m_materials.back()->init();

		return true;
	}

	const bool Renderer3D::createMaterial(const std::string& argMaterialName, const TNAP::EMaterialType argMaterialType, const bool argIncrementNameIfExisting)
	{
		std::string programName{ "" };
		bool foundProgram{ false };
		for (const TNAP::SProgram& program : m_programs)
		{
			if (program.m_type == argMaterialType)
			{
				foundProgram = true;
				programName = program.m_name;
				break;
			}
		}

		if (foundProgram && !programName.empty())
			return createMaterial(argMaterialName, programName, argIncrementNameIfExisting);
		else
			return false;
	}

	void Renderer3D::saveMaterials()
	{
		std::ofstream outputFile;

		outputFile.open("Data/SaveLoad/Materials.csv");

		for (const std::unique_ptr<Material>& mat : m_materials)
		{
			if (!((mat->getProgramHandle() >= 0) && (mat->getProgramHandle() < m_programs.size())))
				continue;

			mat->saveData(outputFile, m_programs.at(mat->getProgramHandle()).m_name);
			outputFile << std::endl;
		}
		outputFile.close();
	}

	void Renderer3D::submitModelMessage(TNAP::Message* const argMessage)
	{
		SubmitModelMessage* const submitModel{ static_cast<SubmitModelMessage*>(argMessage) };

		// Map<ModelHandle, Vector<Pair<Vector<ModelTransform>, Vector<MaterialHandles>>>>

		if (submitModel->m_modelHandle >= m_models.size())
			return;

		/*
		const auto& batch = m_batchRenders.find(submitModel->m_modelHandle);

		if (batch == m_batchRenders.end())
		{
			// New Model
			std::vector<std::pair< std::vector<glm::mat4>, std::vector<size_t> >> temp
			{
				{ std::vector<glm::mat4> { submitModel->m_transform}, std::vector<size_t> { *submitModel->m_materialHandle} }
			};

			m_batchRenders.insert({ submitModel->m_modelHandle, temp });
		}
		else
		{
			// Existing Model
			bool foundMatch = false;
			for (std::pair< std::vector<glm::mat4>, std::vector<size_t> >& pair : m_batchRenders.at(submitModel->m_modelHandle))
			{
				bool matchingMaterials = true;
				for (int i = 0; i < pair.second.size(); i++)
				{
					if (pair.second[i] != (*submitModel->m_materialHandle)[i])
					{
						matchingMaterials = false;
						break;
					}
				}
				if (matchingMaterials)
				{
					pair.first.push_back(submitModel->m_transform);
					foundMatch = true;
					break;
				}
			}

			if (!foundMatch)
			{
				m_batchRenders.at(submitModel->m_modelHandle).emplace_back(std::pair< std::vector<glm::mat4>, std::vector<size_t>>(
					std::vector<glm::mat4> { submitModel->m_transform }, std::vector<size_t> { *submitModel->m_materialHandle }
				));
			}
		} // E.W.
		*/



		for (size_t i = 0; i < submitModel->m_materialHandle->size(); i++)
		{
			Material* mat{ m_materials.at(submitModel->m_materialHandle->at(i)).get() };

			// Check if program batch exists
			{
				const auto& programBatch{ m_batchRenders.find(mat->getProgramHandle()) };
				if (programBatch == m_batchRenders.end())
				{
					// Program does not exist
					m_batchRenders[mat->getProgramHandle()] = ModelBatch();
				}
				assert(m_batchRenders.find(mat->getProgramHandle()) != m_batchRenders.end()); // Somehow failed to create program batch
			}


			//static_assert(std::is_base_of<Entity, EntityType>::value, "addChild: Trying to create entity from non-entity type!");
			// Proram does exist
			{
				const auto& programBatch{ m_batchRenders.find(mat->getProgramHandle()) };
				const auto& modelBatch{ programBatch->second.find(submitModel->m_modelHandle) };
				if (modelBatch == programBatch->second.end())
				{
					// Model does not exist
					programBatch->second[submitModel->m_modelHandle] = { MeshBatch(), MeshBatch() };
				}

				assert(programBatch->second.find(submitModel->m_modelHandle) != programBatch->second.end()); // Somehow failed to create model batch
			}

			// Model does exist
			{
				const auto& programBatch{ m_batchRenders.find(mat->getProgramHandle()) };
				const auto& modelBatch{ programBatch->second.find(submitModel->m_modelHandle) };
				MeshBatch& meshBatch{ mat->getUseTransparency() ? modelBatch->second.second : modelBatch->second.first };

				// Add Transform to previously created meshBatch
				if (i == 0)
					meshBatch.first.emplace_back(submitModel->m_transform);

				if (meshBatch.second.find(i) == meshBatch.second.end())
				{
					// Mesh does not exist
					// MeshData(std::vector<glm::mat4>(), submitModel->m_materialHandle->at(i))
					meshBatch.second.insert({ i, std::vector<MeshData>() });
					//meshBatch.at(i).back().second = submitModel->m_materialHandle->at(i);
				}

				assert(meshBatch.second.find(i) != meshBatch.second.end()); // Somehow failed to create mesh batch

				// Mesh does exist
				bool foundMaterial{ false };
				for (MeshData& meshData : meshBatch.second.at(i))
				{
					if (submitModel->m_materialHandle->at(i) == meshData)
					{
						foundMaterial = true;
						break;
					}
				}
				if (!foundMaterial)
				{
					MeshData newData;
					newData = submitModel->m_materialHandle->at(i);
					meshBatch.second.at(i).emplace_back(newData);
				}
			}

		}

	}

	void Renderer3D::generateMaterialMessage(TNAP::Message* const argMessage)
	{
		GenerateMaterialMessage* const genMessage{ static_cast<GenerateMaterialMessage*>(argMessage) };

		bool createdMaterial{ createMaterial(genMessage->m_materialName, genMessage->m_materialType, true) };

		LogMessage logMessage("Successfully generated new material");
		if (createdMaterial)
		{
			genMessage->m_handle = (m_materials.size() - 1);
			logMessage.m_logType = LogMessage::ELogType::eSuccess;
			TNAP::getEngine()->sendMessage(&logMessage);
		}
		else
		{
			logMessage.m_message = "Failed to generate new material";
			logMessage.m_logType = LogMessage::ELogType::eWarning;
			TNAP::getEngine()->sendMessage(&logMessage);
		}
	}

	void Renderer3D::glfwDropCallBackMessage(TNAP::Message* const argMessage)
	{
		const GLFWDropCallBackMessage* const dropCallBackMessage{ static_cast<GLFWDropCallBackMessage*>(argMessage) };

		for (const std::string& path : dropCallBackMessage->m_paths)
		{
			std::string filePath{ path };

			// Replaces File Path differences to forward slash
			static std::vector<std::string> replacements{ "\\\\", "\\", "//" };
			filePath = TNAP::replaceSubStrings(replacements, filePath, "/");

			std::string fileType{ path };

			fileType.replace(0, fileType.find_last_of('.'), "");
			fileType = TNAP::stringToLowerCase(fileType);

			if (".fbx" == fileType || ".obj" == fileType)
			{
				size_t offset{ filePath.find("Models") };

				if (std::string::npos != offset)
				{
					filePath.replace(0, offset + 7, "");

#if USE_IMGUI
					m_loadWindows.emplace_back(std::make_unique<IMGUIModelLoad>());
					m_loadWindows.back()->setFilePath(filePath);
#else
					loadModel(filePath);
#endif
				}
			}
			else if (".png" == fileType || ".jpg" == fileType || ".jpeg" == fileType || ".tga" == fileType || ".targa" == fileType)
			{
				size_t offset{ filePath.find("Textures") };

				if (std::string::npos != offset)
				{

					filePath.replace(0, offset + 9, "");
#if USE_IMGUI
					m_loadWindows.emplace_back(std::make_unique<IMGUITextureLoad>());
					m_loadWindows.back()->setFilePath(filePath);
#else
					loadTexture(static_cast<ETextureType>(0), filePath);
#endif


				}
			}
		}
	}

	void Renderer3D::render()
	{
		// Configure pipeline settings
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);

		// Uncomment to render in wireframe (can be useful when debugging)
		static bool polyMode{ false };

		static bool held{ false };
#if USE_IMGUI
		if(m_viewportSelected)
#endif
		if (glfwGetKey(getApplication()->getWindow(), GLFW_KEY_P) == GLFW_PRESS && !held)
		{
			held = true;
			polyMode = !polyMode;
		}

		if(glfwGetKey(getApplication()->getWindow(), GLFW_KEY_P) == GLFW_RELEASE)
			held = false;

		if(polyMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Clear buffers from previous frame
		glClearColor(0.5f, 0.5f, 0.5f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if USE_IMGUI
		m_windowFrameBuffer.bind();
#endif
		glViewport(0, 0, static_cast<GLsizei>(m_windowFrameBuffer.getSize().x), static_cast<GLsizei>(m_windowFrameBuffer.getSize().y));

		static bool depthMaskEnabled{ false };
		if (!depthMaskEnabled)
		{
			depthMaskEnabled = true;
			glDepthMask(true);
		}

		// Clear FRAME buffers from previous frame
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Helpers::CheckForGLError();

		/// Creates viewport Projection Matrix
		float aspectRatio{ m_windowFrameBuffer.getSize().x / m_windowFrameBuffer.getSize().y };
		float nearPlane{ 1.0f }, farPlane{ 12000.0f };
		glm::mat4 projection_xform{ glm::perspective(glm::radians(45.0f), aspectRatio, nearPlane, farPlane) };

		/// Creates View Matrix for camera
		glm::mat4 view_xform{ glm::lookAt(Simulation::m_camera->GetPosition(), Simulation::m_camera->GetPosition() + Simulation::m_camera->GetLookVector(), Simulation::m_camera->GetUpVector()) };
		glm::mat4 combined_xform{ projection_xform * view_xform };

		// First loop = opaque			second = transparent
		for (int i = 0; i < 2; i++)
		{
			if (i)
			{
				depthMaskEnabled = false;
				glDepthMask(false);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			for (const auto& programBatch : m_batchRenders)
			{
				const GLuint program{ m_programs.at(programBatch.first).m_program };
				if (program != m_currentProgram)
				{
					glUseProgram(program);
					m_currentProgram = program;
				}

				if (m_programs.at(programBatch.first).m_useLighting)
				{
					for (const std::unique_ptr<SLightData>& lightData : m_nextLights)
						lightData->sendLightData(m_currentProgram);

					SLightData::sendAmountOfLights(m_currentProgram);
					SPointLightData::sendAmountOfLights(m_currentProgram);
					SSpotLightData::sendAmountOfLights(m_currentProgram);
				}

				getSceneManager()->getCurrentScene()->sendShaderData(m_currentProgram);

				/// Create camera ID and then Sends camera forward direction data to shader as Uniform
				const GLuint camera_direcion_id = glGetUniformLocation(program, "camera_direction");
				glUniform3fv(camera_direcion_id, 1, glm::value_ptr(Simulation::m_camera->GetLookVector()));

				const GLuint camera_position_id = glGetUniformLocation(program, "camera_position");
				glUniform3fv(camera_position_id, 1, glm::value_ptr(Simulation::m_camera->GetPosition()));

				/// Create combined xform ID and then Sends Combined Xform data to shader as Uniform
				const GLuint combined_xform_id = glGetUniformLocation(program, "combined_xform");
				glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

				for (const auto& modelBatch : programBatch.second)
				{
					if (modelBatch.first >= m_models.size())
						continue;

					const Model* const model{ &m_models.at(modelBatch.first) };

					const MeshBatch& otBatch{ i ? modelBatch.second.second : modelBatch.second.first }; // ot = opaque / transparent

					// Sort transparnt objects
					if (i)
					{
						/*glm::vec3 camPos = Simulation::m_camera->GetPosition();
						const auto sortL{ [&camPos](const glm::mat4& argFirst, const glm::mat4& argSecond)
						{
							return glm::length(camPos - glm::vec3(argFirst[0][3], argFirst[1][3], argFirst[2][3])) <
								glm::length(camPos - glm::vec3(argSecond[0][3], argSecond[1][3], argSecond[2][3]));
						} };
						std::sort(otBatch.first.begin(), otBatch.first.end(), sortL);*/
					}

					const std::vector<std::unique_ptr<Helpers::Mesh>>& meshes{ model->getMeshVector() };

					const size_t meshBatchSize{ otBatch.first.size() };

					if (meshBatchSize > 0)
					{
						Helpers::CheckForGLError();

						glBindBuffer(GL_ARRAY_BUFFER, m_batchRenderingBuffer);
						glBufferData(GL_ARRAY_BUFFER, meshBatchSize * sizeof(glm::mat4), &otBatch.first.at(0), GL_DYNAMIC_DRAW);
						//glBufferSubData(GL_ARRAY_BUFFER, 0, meshBatchSize * sizeof(glm::mat4), &otBatch.first.at(0));
						//glBufferData(GL_ARRAY_BUFFER, meshBatchSize * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
						Helpers::CheckForGLError();
						//void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
						//void* ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, meshBatchSize * sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

						//assert(NULL != ptr);
						//	
						//// now copy data into memory
						//Helpers::CheckForGLError();
						//std::memcpy(ptr, &otBatch.first.at(0), meshBatchSize * sizeof(glm::mat4));

						//// make sure to tell OpenGL we're done with the pointer
						///*if (!glUnmapBuffer(GL_ARRAY_BUFFER))
						//	continue;*/

						//Helpers::CheckForGLError();

						//glUnmapBuffer(GL_ARRAY_BUFFER);
						glBindBuffer(GL_ARRAY_BUFFER, 0);

						Helpers::CheckForGLError();
					}
					else 
						continue;

					for (const auto& meshBatch : otBatch.second)
					{
						/// Uses Shaders from our program
						Material* mat{ nullptr };

						Helpers::Mesh* mesh = meshes.at(meshBatch.first).get();

						for (const MeshData& meshData : meshBatch.second)
						{
							if (meshData >= m_materials.size())
								mat = m_materials.at(0).get();
							else
								mat = m_materials.at(meshData).get();

							if (mat->getDoubleSided())
								glDisable(GL_CULL_FACE);
							else
								glEnable(GL_CULL_FACE);

							glBindVertexArray(mesh->VAO);

							mat->sendShaderData(m_currentProgram);

							if (meshBatchSize > 1) // Batch Draw
								glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh->elements.size()), GL_UNSIGNED_INT, 0, meshBatchSize);
							else // Normal Draw
								glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->elements.size()), GL_UNSIGNED_INT, (void*)0);

							Helpers::CheckForGLError();

							glBindVertexArray(0);
						}
					}
				}
			}
		}

#if USE_IMGUI
		m_windowFrameBuffer.unbind();
#endif

		m_nextLights.clear();
		m_batchRenders.clear();

		Helpers::CheckForGLError();
	}

#if USE_IMGUI
	void Renderer3D::imGuiRenderShelf()
	{
		static std::vector<std::pair<std::string, std::function<void()>>> shelves
		{
			{
				"Textures", [&]()
				{
					static int iconSize{ 32 };
					ImGui::SliderInt("Icon Size", &iconSize, 32, 256);

					if (iconSize <= 0)
						iconSize = 1;

					int amount{ static_cast<int>((ImGui::GetWindowWidth() - (iconSize * 2)) / iconSize) };
					amount = std::max(amount, 1);
					ImGui::Spacing();

					for (int i = 0; i < textureTypeNames.size(); i++)
					{
						const std::vector<STextureData>& textures{ m_textures.at(i) };
						if (ImGui::CollapsingHeader(textureTypeNames.at(i).c_str()))
						{
							for (int j = 0; j < textures.size(); j++)
							{
								if (j % amount != 0)
									ImGui::SameLine(0.0f, 0.0f);

								ImGui::ImageButton((ImTextureID)textures[j].m_textureBinding, ImVec2(static_cast<float>(iconSize), static_cast<float>(iconSize)), ImVec2(0, 1), ImVec2(1, 0), 1);

								if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
								{
									ImGui::SetDragDropPayload("TEXTURE_CELL", &std::pair<ETextureType, size_t>(static_cast<ETextureType>(i), j), sizeof(std::pair<ETextureType, size_t>));
									ImGui::Image((ImTextureID)textures[j].m_textureBinding, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
									ImGui::Text(("Texture Path: " + textures[j].m_filePath).c_str());
									ImGui::Text(("Texture Type: " + textureTypeNames.at(i)).c_str());
									ImGui::Text(("Texture Handle: " + std::to_string(j)).c_str());
									ImGui::EndDragDropSource();
								}
							}
						}
					}
					
				}
			},

			{
				"Materials", [&]() 
				{
					if (ImGui::Button("Save Materials"))
					{
						saveMaterials();
					}

					ImGui::Columns(3, "materials"); // 3-ways, with border

					ImGui::Separator();
					ImGui::Text("Mat"); ImGui::NextColumn();
					ImGui::Text("Name"); ImGui::NextColumn();
					ImGui::Text("Handle"); ImGui::NextColumn();
					ImGui::Separator();

					for (size_t i = 0; i < m_materials.size(); i++)
					{
						const size_t textureBinding{ m_textures.at(static_cast<size_t>(ETextureType::eAlbedo)).at(m_mapTextures.at(ETextureType::eAlbedo).at("MatTexture.png")).m_textureBinding };

						//ImGui::PushID(i);
						const std::string id{ std::to_string(i) };
						ImGui::PushID(id.c_str(), id.c_str() + id.size());
						ImGui::ImageButton((ImTextureID)textureBinding, ImVec2(16, 16), ImVec2(0, 1), ImVec2(1, 0), 1);
						ImGui::PopID();

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("MATERIAL_CELL", &i, sizeof(i));
							ImGui::Image((ImTextureID)textureBinding, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::Text(("Material Name: " + m_materials.at(i)->getName()).c_str());
							ImGui::Text(("Material Type: " + std::to_string(static_cast<int>(m_materials.at(i)->getMaterialType()))).c_str());
							ImGui::EndDragDropSource();
						}
						ImGui::NextColumn();

						ImGui::Text(m_materials[i]->getName().c_str()); ImGui::NextColumn();
						ImGui::Text(std::to_string(i).c_str()); ImGui::NextColumn();

						ImGui::Separator();
					}

					ImGui::Columns(1, "materials");
				} 
			},

			{
				"Models", [&]() 
				{ 	
					ImGui::Columns(6, "models"); // 6-ways, with border
					ImGui::Separator();
					ImGui::Text("Model"); ImGui::NextColumn();
					ImGui::Text("Name"); ImGui::NextColumn();
					ImGui::Text("Path"); ImGui::NextColumn();
					ImGui::Text("Handle"); ImGui::NextColumn();
					ImGui::Text("Mesh Count"); ImGui::NextColumn();
					ImGui::Text("Mat Count"); ImGui::NextColumn();
					ImGui::Separator();

					for (const auto& mapModel : m_mapModels)
					{
						const size_t textureBinding{ m_textures.at(static_cast<size_t>(ETextureType::eAlbedo)).at(m_mapTextures.at(ETextureType::eAlbedo).at("ModelTexture.png")).m_textureBinding };

						//ImGui::PushID(mapModel.second);
						const std::string id{ std::to_string(mapModel.second) };
						ImGui::PushID(id.c_str(), id.c_str() + id.size());
						ImGui::ImageButton((ImTextureID)textureBinding, ImVec2(16, 16), ImVec2(0, 1), ImVec2(1, 0), 1);
						ImGui::PopID();

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("MODEL_CELL", &mapModel.second, sizeof(mapModel.second));
							ImGui::Image((ImTextureID)textureBinding, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::Text(("Model Name: " + mapModel.first).c_str());
							ImGui::Text(("Model Path: " + m_models.at(mapModel.second).getFilePath()).c_str());
							ImGui::Text(("Model Handle: " + std::to_string(mapModel.second)).c_str());
							ImGui::EndDragDropSource();
						}
						ImGui::NextColumn();

						ImGui::Text(mapModel.first.c_str()); ImGui::NextColumn();
						ImGui::Text(m_models.at(mapModel.second).getFilePath().c_str()); ImGui::NextColumn();
						ImGui::Text(std::to_string(mapModel.second).c_str()); ImGui::NextColumn();
						ImGui::Text(std::to_string(m_models.at(mapModel.second).getMeshVector().size()).c_str()); ImGui::NextColumn();
						ImGui::Text(std::to_string(m_models.at(mapModel.second).getUniqueMaterialIndicesCount()).c_str()); ImGui::NextColumn();

						ImGui::Separator();
					}

					ImGui::Columns(1, "models"); 
				} 
			}
		};

		static bool shelfOpen{ true };
		ImGui::Begin("Shelf", &shelfOpen, ImGuiWindowFlags_MenuBar);
		{
			static bool showMaterials{ true };
			static bool showModels{ true };

			static std::array<bool, static_cast<int>(TNAP::ETextureType::eCount)> headerOpen{ true, true, true, true, true, true };

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Create"))
				{
					static std::string name{ "" };
					ImGui::InputText("Name", &name);

					if (ImGui::BeginMenu("Texture"))
					{
						static std::string textureFilePath{ "" };
						static TNAP::ETextureType textureType{ TNAP::ETextureType::eAlbedo };

						ImGui::InputText("Filepath", &textureFilePath);

						if (ImGui::BeginCombo("Texture Type", textureTypeNames.at(static_cast<int>(textureType)).c_str()))
						{
							for (int i = 0; i < textureTypeNames.size(); i++)
							{
								if (ImGui::Selectable(textureTypeNames.at(i).c_str()))
									textureType = static_cast<TNAP::ETextureType>(i);
							}
							ImGui::EndCombo();
						}

						if (ImGui::Button("Create"))
						{
							if (!textureFilePath.empty())
							{
								loadTexture(textureType, textureFilePath, name);
								textureFilePath.clear();
								name.clear();
							}
						}

						ImGui::EndMenu();
					}

					static std::array<std::string, static_cast<int>(TNAP::EMaterialType::eCount)> materialNames{ "Unlit", "UnlitTexture", "PBR" };

					if (ImGui::BeginMenu("Material"))
					{
						static TNAP::EMaterialType materialType{ TNAP::EMaterialType::eUnlit };

						if (ImGui::BeginCombo("Material Type", materialNames.at(static_cast<int>(materialType)).c_str()))
						{
							for (int i = 0; i < materialNames.size(); i++)
							{
								if (ImGui::Selectable(materialNames.at(i).c_str()))
									materialType = static_cast<TNAP::EMaterialType>(i);
							}
							ImGui::EndCombo();
						}

						if (ImGui::Button("Create"))
						{
							if (!name.empty())
							{
								createMaterial(name, materialType, true);
								name.clear();
							}
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Show"))
				{
					ImGui::MenuItem("Materials", NULL, &showMaterials);
					ImGui::MenuItem("Models", NULL, &showModels);

					if (ImGui::BeginMenu("Textures"))
					{
						if (ImGui::Button("Invert"))
						{
							for (int i = 0; i < headerOpen.size(); i++)
								headerOpen.at(i) = !headerOpen.at(i);
						}

						ImGui::SameLine();

						if (ImGui::Button("Hide"))
							headerOpen.fill(false);

						ImGui::SameLine();

						if (ImGui::Button("Open"))
							headerOpen.fill(true);

						for (int i = 0; i < textureTypeNames.size(); i++)
							ImGui::MenuItem(textureTypeNames.at(i).c_str(), NULL, &headerOpen.at(i));

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			static std::function<void()>* currentShelf{ nullptr };
			ImGui::BeginChild("Shelf Hierarchy", ImVec2(100, 0), true);
			{
				for (int i = 0; i < shelves.size(); i++)
				{
					if (ImGui::Selectable(shelves.at(i).first.c_str()))
						currentShelf = &shelves.at(i).second;

				}
			}
			ImGui::EndChild();

			ImGui::SameLine();

			ImGui::BeginChild("Shelf Data", ImVec2(0, 0), true);
			{
				if (nullptr != currentShelf)
					(*currentShelf)();
			}

			ImGui::EndChild();
		}
		ImGui::End();

	}

	void Renderer3D::imGuiRenderViewport()
	{
		const glm::vec2& windowSize{ getApplication()->getWindowSize() };
		GLFWwindow* const window{ getApplication()->getWindow() };

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			static bool viewportOpen = true;
			ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_None;
			flags |= ImGuiWindowFlags_NoMove;
			//flags |= ImGuiWindowFlags_NoResize;
			flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

			ImGui::Begin("Viewport", &viewportOpen, flags);
			{
				ImGui::PopStyleVar(3);

				m_viewportSelected = ImGui::IsWindowFocused();

				bool mousePressed{ false };
				if (m_viewportSelected)
				{
					if (glfwGetMouseButton(window, 0) == GLFW_PRESS)
					{
						glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						mousePressed = true;
					}
				}

				if (!mousePressed)
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

				Simulation::m_camera->setActive(m_viewportSelected && mousePressed);

				ImVec2 RegionSize{ ImGui::GetContentRegionAvail() };

				if ((RegionSize.x != windowSize.x) || (RegionSize.y != windowSize.y))
				{
					getApplication()->setWindowSize(glm::vec2(RegionSize.x, RegionSize.y));

					m_windowFrameBuffer.resize(windowSize);

					Helpers::CheckForGLError();
				}

				ImGui::Image((ImTextureID)m_windowFrameBuffer.getColourAttachment(), ImVec2(windowSize.x, windowSize.y), ImVec2(0, 1), ImVec2(1, 0));
			}
			ImGui::End();
		}
	}
#endif

	Renderer3D::Renderer3D()
	{
		m_textures.resize(static_cast<int>(ETextureType::eCount));
	}

	Renderer3D::~Renderer3D()
	{
		for(const SProgram& program : m_programs)
			glDeleteProgram(program.m_program);
	}

	void Renderer3D::init()
	{
		

		m_windowFrameBuffer.init();
		const glm::vec2& windowSize{ getApplication()->getWindowSize() };
		m_windowFrameBuffer.resize(windowSize);

		Helpers::CheckForGLError();
		
		glGenBuffers(1, &m_batchRenderingBuffer);

		GLuint whiteText{ 0 };
		glGenTextures(1, &whiteText);
		glBindTexture(GL_TEXTURE_2D, whiteText);

		static GLbyte whitePixels[4]{ 255, 255, 255, 255 };

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<void*>(whitePixels));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Helpers::CheckForGLError();

		GLuint normalText{ 0 };
		glGenTextures(1, &normalText);
		glBindTexture(GL_TEXTURE_2D, normalText);

		static GLbyte normalPixels[4]{ 127, 127, 255, 255 };

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<void*>(normalPixels));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Helpers::CheckForGLError();

		// std::vector<TNAP::STextureData>
		for (int i = 0; i < m_textures.size(); i++)
		{
			std::unique_ptr<Helpers::ImageLoader> texture{ std::make_unique<Helpers::ImageLoader>() };
			if (static_cast<int>(ETextureType::eNormal) == i)
			{
				texture->SetData(1, 1, &normalPixels[0]);

				m_textures.at(i).push_back(STextureData(std::move(texture), normalText, ""));
			}
			else
			{
				texture->SetData(1, 1, &whitePixels[0]);

				m_textures.at(i).push_back(STextureData(std::move(texture), whiteText, ""));
			}
		}

		loadShaders();
		loadMaterials("Materials.csv");

		loadModel("ErrorMesh.fbx");
		loadModel("Primitives/Cube.fbx");
		loadModel("Primitives/Sphere.fbx");
		loadModel("Primitives/Cylinder.fbx");
		loadModel("Primitives/Plane.fbx");
		loadModel("Primitives/Cone.fbx");
		
		loadTexture(TNAP::ETextureType::eAlbedo, "Editor/MissingTexture.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "Editor/MatTexture.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Editor/ModelTexture.png");


		///
		// TEMP
		///
		//loadModel("fire-elemental\\ostatni test1.fbx"); 
		//loadModel("creature-evolution-2\\EVOLUTION_02_POSED.fbx"); 
		//loadModel("desert-trooper-rigged\\SandTrooper.fbx");
		//loadModel("low-poly-spider-tank\\Part_01xxx.fbx"); 
		//loadModel("three-eyed-demon\\Third_Eyexxx.OBJ");

		loadTexture(TNAP::ETextureType::eAlbedo, "birdplane.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "creature-evolution-2/Default Material1_Flattened_Diffuse.png");

		loadTexture(TNAP::ETextureType::eAlbedo, "aqua_pig_2K.png");

		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix/Tex_Ride_FengHuang_01a_D_A.tga.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix/Tex_Ride_FengHuang_01a_E.tga.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix/Tex_Ride_FengHuang_01b_D_A.tga.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix/Tex_Ride_FengHuang_01b_E.tga.png");

		loadTexture(TNAP::ETextureType::eAlbedo, "desert-trooper-rigged/LowerBody_Base_Color.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "desert-trooper-rigged/Helmet_Base_Color.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "desert-trooper-rigged/UpperBody_Base_Color.jpg");

		loadTexture(TNAP::ETextureType::eAlbedo, "fire-elemental/diffuse fire_elemntal.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "low-poly-spider-tank/Part_01x_albedo.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "AR/AssaultRifleModel_Albedo.tga");
		loadTexture(TNAP::ETextureType::eEmission, "three-eyed-demon/Third_Eyex_Albedo.jpg");

	}

	void Renderer3D::update()
	{
		render();
	}

	void Renderer3D::sendMessage(TNAP::Message* const argMessage)
	{
		if (nullptr == argMessage)
			return;

		switch (argMessage->getMessageType())
		{
		case Message::EMessageType::eGetTextureMessage:
		{
			GetTextureMessage* const textureMessage{ static_cast<GetTextureMessage*>(argMessage) };

			textureMessage->m_textureData = &m_textures.at(static_cast<int>(textureMessage->m_textureInfo.first)).at(textureMessage->m_textureInfo.second);
		}
		break;

		case Message::EMessageType::eLoadTextureMessage:
		{
			LoadTextureMessage* const textureMessage{ static_cast<LoadTextureMessage*>(argMessage) };

			std::pair<bool, size_t> textureReturn{ loadTexture(textureMessage->m_loadInfo.first, textureMessage->m_loadInfo.second, textureMessage->m_textureName) };

			if (textureReturn.first)
			{
				textureMessage->m_loadedSuccessfully = true;
				//const size_t textureHandle{ m_mapTextures.at(textureMessage->m_loadInfo.first).at(textureMessage->m_loadInfo.second) };

				textureMessage->m_textureData = &m_textures.at(static_cast<size_t>(textureMessage->m_loadInfo.first)).at(textureReturn.second);
				textureMessage->m_textureHandle = textureReturn.second;
			}
		}
		break;

		case Message::EMessageType::eSubmitModelMessage:
			submitModelMessage(argMessage);
			break;

		case Message::EMessageType::eGetMaterialMessage:
		{
			GetMaterialMessage* const materialMessage{ static_cast<GetMaterialMessage*>(argMessage) };

			for (int i = 0; i < materialMessage->m_materialHandle->size(); i++)
			{
				materialMessage->m_materialVector.emplace_back(m_materials.at(materialMessage->m_materialHandle->at(i)).get());
			}
		}
		break;

		case Message::EMessageType::eLoadModelMessage:
		{
			LoadModelMessage* const loadModelMessage{ static_cast<LoadModelMessage*>(argMessage) };

			loadModelMessage->m_modelHandle = loadModel(loadModelMessage->m_modelFilepath, loadModelMessage->m_modelName).second;
			loadModelMessage->m_materialHandles = m_models.at(loadModelMessage->m_modelHandle).getDefaultMaterialHandles();
		}
		break;

		case Message::EMessageType::eGenerateMaterialMessage:
			generateMaterialMessage(argMessage);
			break;

		case Message::EMessageType::eGLFWDropCallBackMessage:
			glfwDropCallBackMessage(argMessage);
			break;

		case Message::EMessageType::eGetModelInfoMessage:
		{
			GetModelInfoMessage* const getModelInfoMessage{ static_cast<GetModelInfoMessage*>(argMessage) };

			getModelInfoMessage->m_filepath = m_models.at(getModelInfoMessage->m_modelHandle).getFilePath();
			getModelInfoMessage->m_defaultMaterialHandles = m_models.at(getModelInfoMessage->m_modelHandle).getDefaultMaterialHandles();
		}
		break;

		case Message::EMessageType::eGetMaterialHandleMessage:
		{
			GetMaterialHandleMessage* const getMaterialHandleMessage{ static_cast<GetMaterialHandleMessage*>(argMessage) };
			
			const auto& materialFind{ m_mapMaterials.find(getMaterialHandleMessage->m_materialName) };
			if (materialFind != m_mapMaterials.end())
				getMaterialHandleMessage->m_materialHandle = m_mapMaterials.at(getMaterialHandleMessage->m_materialName);
		}
		break;

		case Message::EMessageType::eSubmitLightDataMessage:
		{
			SubmitLightDataMessage* const lightDataMessage{ static_cast<SubmitLightDataMessage*>(argMessage) };

			m_nextLights.emplace_back(std::move(lightDataMessage->m_data));
		}
		break;

		default:
			break;
		}
	}

#if USE_IMGUI
	void Renderer3D::imGuiRender()
	{
		for (size_t i = 0; i < m_loadWindows.size(); ++i)
		{
			if (!m_loadWindows.at(i)->getShouldClose())
				m_loadWindows.at(i)->ImGuiRenderLoad(i);
		}

		bool hasUpdated{ true };
		do
		{
			hasUpdated = false;
			for (size_t i = 0; i < m_loadWindows.size(); ++i)
			{
				if (m_loadWindows.at(i)->getShouldClose())
				{
					hasUpdated = true;
					std::swap(m_loadWindows.at(i), m_loadWindows.back());
					m_loadWindows.pop_back();
				}
			}
		} while (hasUpdated);

		imGuiRenderViewport();

		imGuiRenderShelf();
	}

	void Renderer3D::imGuiRenderMaterial(const size_t argMaterialHandle)
	{
		if(argMaterialHandle >= 0 && argMaterialHandle < m_materials.size())
			m_materials[argMaterialHandle]->imGuiRender();
	}

#endif
}