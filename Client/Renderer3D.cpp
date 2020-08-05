#include "Renderer3D.hpp"

#include <array>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "ImGuiInclude.hpp"
#include "ExternalLibraryHeaders.h"
#include "Engine.hpp"
#include "Mesh.h"
#include "Simulation.h"
#include "GetTextureMessage.hpp"
#include "SubmitModelMessage.hpp"
#include "GetMaterialMessage.hpp"
#include "LoadModelMessage.hpp"
#include "LogMessage.hpp"
#include "GenerateMaterialMessage.hpp"

#include "UnlitTexture.hpp"
#include "PBR.hpp"

namespace TNAP {
	GLFWwindow* Renderer3D::s_window{ nullptr };

	const size_t Renderer3D::loadModel(const std::string& argFilePath)
	{
		LogMessage logMessage("");
		if (argFilePath.empty())
		{
			logMessage.m_message = "[Model] Filepath is empty, returning handle 0";
			logMessage.m_logType = LogMessage::ELogType::eWarning;
			TNAP::getEngine()->sendMessage(&logMessage);

			return 0;
		}


		std::string filePath{ argFilePath };

		// Replaces File Path differences to forward slash
		static std::array<std::string, 3> replacements{ "\\\\", "\\", "//" };
		for (const std::string& replace : replacements)
		{
			size_t index{ 0 };
			index = filePath.find(replace);
			while (std::string::npos != index)
			{
				filePath.replace(index, replace.size(), "/");
				index = filePath.find(replace, index + 2);
			}
		}

		
		const auto& modelFind{ m_mapModels.find(filePath) };
		if (modelFind != m_mapModels.end())
		{
			logMessage.m_message = "[Model] " + filePath + " is already loaded";
			logMessage.m_logType = LogMessage::ELogType::eInfo;
			TNAP::getEngine()->sendMessage(&logMessage);
			return modelFind->second;
		}

		m_models.emplace_back(TNAP::Model());

		if (!m_models.back().loadFromFile("Data/Models/" + filePath))
		{
			logMessage.m_message = "[Model] " + filePath + " could not load, returning handle 0";
			logMessage.m_logType = LogMessage::ELogType::eWarning;
			TNAP::getEngine()->sendMessage(&logMessage);

			// If file fails to load, return first model
			return 0;
		}

		m_mapModels.insert({ filePath, m_models.size() - 1 });

		// Setup materials
		for (int i = 0; i < m_models.back().getUniqueMaterialIndicesCount(); i++)
		{
			std::string name = filePath + "_" + std::to_string(i);
			const auto& it{ m_mapMaterials.find(name) };
			if (it != m_mapMaterials.end())
			{
				// material already Created
				m_models.back().addDefaultMaterialHandle(it->second);
				continue;
			}

			m_materials.emplace_back(std::make_unique<UnlitTexture>());
			m_mapMaterials.insert({ name, m_materials.size() - 1 });

			m_materials.back()->m_programHandle = m_mapPrograms.at("Unlit");
			m_materials.back()->m_name = name;
			m_models.back().addDefaultMaterialHandle(m_mapMaterials.size() - 1);
		}

		logMessage.m_message = "[Model] " + filePath + " loaded successfully";
		logMessage.m_logType = LogMessage::ELogType::eSuccess;
		TNAP::getEngine()->sendMessage(&logMessage);

		return (m_models.size() - 1);
	}

	void Renderer3D::loadShaders()
	{
		std::ifstream savedShaders;

		savedShaders.open("Data/SaveLoad/Shaders.csv");

		std::string name;
		std::string type;
		std::string vert;
		std::string frag;
		
		while (savedShaders.good())
		{
			std::getline(savedShaders, name, ',');
			std::getline(savedShaders, type, ',');
			std::getline(savedShaders, vert, ',');
			std::getline(savedShaders, frag, '\n');
			createShader(static_cast<EMaterialType>(std::stoi(type)), name, vert, frag);
		}

		savedShaders.close();
	}

	const bool Renderer3D::loadTexture(const TNAP::ETextureType argType, const std::string& argFilePath)
	{
		if (argFilePath.empty())
			return false;

		if (m_mapTextures.find(argType) == m_mapTextures.end())
			m_mapTextures.insert({ argType, std::unordered_map<std::string, size_t>() });


		std::string filePath{ argFilePath };

		// Replaces File Path differences to forward slash
		static std::array<std::string, 3> replacements{ "\\\\", "\\", "//" };
		for (const std::string& replace : replacements)
		{
			size_t index{ 0 };
			index = filePath.find(replace);
			while (std::string::npos != index)
			{
				filePath.replace(index, replace.size(), "/");
				index = filePath.find(replace, index + 2);
			}
		}

		LogMessage logMessage("");
		if (m_mapTextures.at(argType).find(filePath) != m_mapTextures.at(argType).end())
		{
			// Log Texture Already Loaded
			logMessage.m_message = "[Texture] " + filePath + " is already loaded";
			logMessage.m_logType = LogMessage::ELogType::eInfo;
			TNAP::getEngine()->sendMessage(&logMessage);
			return false;
		}

		std::unique_ptr<Helpers::ImageLoader> texture{ std::make_unique<Helpers::ImageLoader>() };

		if (!texture->Load("Data/Textures/" + filePath))
		{
			// Log Missing texture
			logMessage.m_message = "[Texture] " + filePath + " could not load";
			logMessage.m_logType = LogMessage::ELogType::eWarning;
			TNAP::getEngine()->sendMessage(&logMessage);
			return false;
		}

		// Log Loaded Texture
		logMessage.m_message = "[Texture] " + filePath + " is loaded successfully";
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

		m_textures.at(static_cast<int>(argType)).push_back({ std::move(texture), textureRef });
		m_mapTextures.at(argType).insert({ filePath, m_textures.at(static_cast<int>(argType)).size() - 1 });

		return true;
	}

	void Renderer3D::loadMaterials(const std::string& argFilePath)
	{
		if (argFilePath.empty())
			return;

		createMaterial("DefaultMaterial_0", "Unlit");

		std::ifstream savedMaterials;

		savedMaterials.open("Data/SaveLoad/" + argFilePath);

		std::string name;
		std::string shaderName;
		std::string info;

		while (savedMaterials.good())
		{
			std::getline(savedMaterials, name, ',');
			std::getline(savedMaterials, shaderName, ',');
			std::getline(savedMaterials, info);

			if (createMaterial(name, shaderName))
				m_materials.at(m_mapMaterials.at(name))->setData(info);
		}

		savedMaterials.close();
	}

	const bool Renderer3D::createShader(const EMaterialType argType, const std::string& argShaderName, const std::string& argVertexShaderPath, const std::string& argFragmentShaderPath)
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

		SProgram newProgram(program, argType, argShaderName, argVertexShaderPath, argFragmentShaderPath);

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
			if (!argIncrementNameIfExisting)
				return true;
			else
			{
				// Removes _IDNum from end of name
				size_t matNameIndex{ materialName.find_last_of("_") };
				materialName.erase(matNameIndex);
				matNameIndex = 0;

				while (m_mapMaterials.find(materialName + "_" + std::to_string(matNameIndex)) != m_mapMaterials.end())
					matNameIndex++;

				materialName += "_" + std::to_string(matNameIndex);
			}
		}

		switch (m_programs.at(m_mapPrograms.at(argShaderName)).m_type)
		{
		case EMaterialType::eUnlit:
		{
			m_materials.emplace_back(std::make_unique<Material>());
		}
		break;

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
			break;
		}

		m_mapMaterials.insert({ materialName, m_materials.size() - 1 });

		m_materials.back()->m_programHandle = m_mapPrograms.at(argShaderName);
		m_materials.back()->m_name = materialName;

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

	Renderer3D::Renderer3D()
	{
		m_textures.resize(static_cast<int>(ETextureType::eCount));
	}

	Renderer3D::~Renderer3D()
	{
		for(const SProgram program : m_programs)
			glDeleteProgram(program.m_program);


		glfwDestroyWindow(s_window);
		glfwTerminate();
	}

	void Renderer3D::init()
	{
		// Use the helper function to set up GLFW, GLEW and OpenGL
		s_window = Helpers::CreateGLFWWindow(m_windowSize.x, m_windowSize.y, "TNAP");
		if (!s_window)
			return;


		loadShaders();
		loadMaterials("Materials.csv");

		m_windowFrameBuffer.init();
		m_windowFrameBuffer.resize(glm::vec2( m_windowSize.x, m_windowSize.y));
		Helpers::CheckForGLError();

		glfwSetInputMode(s_window, GLFW_STICKY_KEYS, GLFW_TRUE);

		
		glGenBuffers(1, &batchRenderingBuffer);


		GLuint whiteText{ 0 };
		glGenTextures(1, &whiteText);
		glBindTexture(GL_TEXTURE_2D, whiteText);

		static GLbyte pixels[4]{ 255, 255, 255, 255 };

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<void*>(pixels));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Helpers::CheckForGLError();


		for (auto& textType : m_textures)
		{
			std::unique_ptr<Helpers::ImageLoader> texture{ std::make_unique<Helpers::ImageLoader>() };
			texture->SetData(1, 1, &pixels[0]);

			textType.push_back({ std::move(texture), whiteText });
		}

		loadModel("ErrorMesh.fbx");
		loadModel("Primitives\\Cube.fbx");
		loadModel("Primitives\\Sphere.fbx");
		loadModel("Primitives\\Cylinder.fbx");
		loadModel("Primitives\\Plane.fbx");
		loadModel("Primitives\\Cone.fbx");
		
		loadTexture(TNAP::ETextureType::eAlbedo, "Editor\\MissingTexture.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "Editor\\MatTexture.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Editor\\ModelTexture.png");


		///
		// TEMP
		///
		//loadModel("fire-elemental\\ostatni test1.fbx"); 
		//loadModel("creature-evolution-2\\EVOLUTION_02_POSED.fbx"); 
		//loadModel("desert-trooper-rigged\\SandTrooper.fbx");
		//loadModel("low-poly-spider-tank\\Part_01xxx.fbx"); 
		//loadModel("three-eyed-demon\\Third_Eyexxx.OBJ");

		loadTexture(TNAP::ETextureType::eAlbedo, "birdplane.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "creature-evolution-2\\Default Material1_Flattened_Diffuse.png");

		loadTexture(TNAP::ETextureType::eAlbedo, "aqua_pig_2K.png");

		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix\\Tex_Ride_FengHuang_01a_D_A.tga.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix\\Tex_Ride_FengHuang_01a_E.tga.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix\\Tex_Ride_FengHuang_01b_D_A.tga.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "Phoenix\\Tex_Ride_FengHuang_01b_E.tga.png");

		loadTexture(TNAP::ETextureType::eAlbedo, "desert-trooper-rigged\\LowerBody_Base_Color.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "desert-trooper-rigged\\Helmet_Base_Color.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "desert-trooper-rigged\\UpperBody_Base_Color.jpg");

		loadTexture(TNAP::ETextureType::eAlbedo, "fire-elemental\\diffuse fire_elemntal.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "low-poly-spider-tank\\Part_01x_albedo.jpg");
		loadTexture(TNAP::ETextureType::eAlbedo, "AR\\AssaultRifleModel_Albedo.tga");
		loadTexture(TNAP::ETextureType::eEmission, "three-eyed-demon\\Third_Eyex_Albedo.jpg");
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

			const std::pair<std::unique_ptr<Helpers::ImageLoader>, GLuint>& textureData{ m_textures.at(static_cast<int>(textureMessage->m_textureInfo.first)).at(textureMessage->m_textureInfo.second) };

			textureMessage->m_textureData = textureData.first.get();
			textureMessage->m_textureBinding = textureData.second;
		}
		break;

		case Message::EMessageType::eSubmitModelMessage:
		{
			SubmitModelMessage* const submitModel{ static_cast<SubmitModelMessage*>(argMessage) };

			// Map<ModelHandle, Vector<Pair<Vector<ModelTransform>, Vector<MaterialHandles>>>>

			if (submitModel->m_modelHandle >= m_models.size())
				return;

			if (m_batchRenders.find(submitModel->m_modelHandle) == m_batchRenders.end())
			{
				// New Model
				std::vector<glm::mat4> transforms
				{
					submitModel->m_transform
				};

				std::vector<size_t> materialHandle
				{
					*submitModel->m_materialHandle
				};

				std::vector<std::pair< std::vector<glm::mat4>, std::vector<size_t> >> temp
				{
					{transforms, materialHandle}
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
					std::vector<glm::mat4> transforms
					{
						submitModel->m_transform
					};

					std::vector<size_t> materialHandle
					{
						*submitModel->m_materialHandle
					};

					m_batchRenders.at(submitModel->m_modelHandle).emplace_back(std::pair< std::vector<glm::mat4>, std::vector<size_t>>(transforms, materialHandle));
				}
			}

		}
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

			loadModelMessage->m_modelHandle = loadModel(loadModelMessage->m_modelFilepath);
			loadModelMessage->m_materialHandles = m_models.at(loadModelMessage->m_modelHandle).getDefaultMaterialHandles();
		}
		break;

		case Message::EMessageType::eGenerateMaterialMessage:
		{
			GenerateMaterialMessage* const genMessage{ static_cast<GenerateMaterialMessage*>(argMessage) };

			bool createdMaterial{ false };
			switch (genMessage->m_materialType)
			{
			case TNAP::EMaterialType::eUnlit:
				createdMaterial = createMaterial(genMessage->m_materialName, "Unlit", true);
				break;

			case TNAP::EMaterialType::eUnlitTexture:
				createdMaterial = createMaterial(genMessage->m_materialName, "UnlitTexture", true);
				break;

			case TNAP::EMaterialType::ePBR:
				createdMaterial = createMaterial(genMessage->m_materialName, "PBR", true);
				break;
			default:
				break;
			}

			LogMessage logMessage("Successfully generated new material");
			if (createdMaterial)
			{
				genMessage->m_handle = m_materials.size() - 1;
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
		break;

		default:
			break;
		}

	}



	void Renderer3D::render()
	{
		// Configure pipeline settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Uncomment to render in wireframe (can be useful when debugging)
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Clear buffers from previous frame
		glClearColor(0.5f, 0.5f, 0.5f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
		
		m_windowFrameBuffer.bind();
		glViewport(0, 0, m_windowFrameBuffer.getSize().x, m_windowFrameBuffer.getSize().y);

		// Clear FRAME buffers from previous frame
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Helpers::CheckForGLError();
		/// Creates and Gets Viewport size
		GLint viewportDimensions[4];
		glGetIntegerv(GL_VIEWPORT, viewportDimensions);

		/// Creates viewport Projection Matrix
		float aspectRatio = m_windowFrameBuffer.getSize().x / m_windowFrameBuffer.getSize().y;
		float nearPlane{ 1.0f }, farPlane{ 12000.0f };
		glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspectRatio, nearPlane, farPlane);

		/// Creates View Matrix for camera
		glm::mat4 view_xform = glm::lookAt(Simulation::m_camera->GetPosition(), Simulation::m_camera->GetPosition() + Simulation::m_camera->GetLookVector(), Simulation::m_camera->GetUpVector());
		glm::mat4 combined_xform = projection_xform * view_xform;

		for (const auto& modelBatch : m_batchRenders)
		{
			if (modelBatch.first >= m_models.size())
				continue;

			const Model* const model{ &m_models.at(modelBatch.first) };

			// Transforms, MaterialHandles
			for (const std::pair<std::vector<glm::mat4>, std::vector<size_t>>& batch : modelBatch.second)
			{
				if (batch.first.size() <= 0)
					continue;

				const std::vector<std::unique_ptr<Helpers::Mesh>>& meshes{ model->getMeshVector() };
				
				for (int i = 0; i < meshes.size(); i++)
				{
					/// Uses Shaders from our program
					Material* mat{ nullptr };
					
					if (meshes[i]->materialIndex >= batch.second.size())
						mat = m_materials.at(0).get();
					else
						mat = m_materials.at(batch.second.at(meshes[i]->materialIndex)).get();

					GLuint program{ m_programs[mat->getProgramHandle()].m_program };

					if (program != m_currentProgram)
					{
						glUseProgram(program);
						m_currentProgram = program;
					}

					/// Create camera ID and then Sends camera forward direction data to shader as Uniform
					GLuint camera_direcion_id = glGetUniformLocation(program, "camera_direction");
					glUniform3fv(camera_direcion_id, 1, glm::value_ptr(Simulation::m_camera->GetLookVector()));

					/// Create combined xform ID and then Sends Combined Xform data to shader as Uniform
					GLuint combined_xform_id = glGetUniformLocation(program, "combined_xform");
					glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));


					glBindVertexArray(meshes[i]->VAO);

					mat->sendShaderData(m_currentProgram);

					glBindBuffer(GL_ARRAY_BUFFER, batchRenderingBuffer);
					glBufferData(GL_ARRAY_BUFFER, batch.first.size() * sizeof(glm::mat4), &batch.first.at(0), GL_DYNAMIC_DRAW);

					// Set attribute pointers for matrix (4 times vec4)
					glEnableVertexAttribArray(3);
					glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
					glEnableVertexAttribArray(4);
					glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
					glEnableVertexAttribArray(5);
					glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
					glEnableVertexAttribArray(6);
					glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

					glVertexAttribDivisor(3, 1);
					glVertexAttribDivisor(4, 1);
					glVertexAttribDivisor(5, 1);
					glVertexAttribDivisor(6, 1);

					if (batch.first.size() > 1) // Batch Draw
					{
						glDrawElementsInstanced(GL_TRIANGLES, meshes[i]->elements.size(), GL_UNSIGNED_INT, 0, batch.first.size());

						Helpers::CheckForGLError();
					}
					else // Normal Draw
					{

						glDrawElements(GL_TRIANGLES, meshes[i]->elements.size(), GL_UNSIGNED_INT, (void*)0);
					}

					glBindVertexArray(0);
				}

			}

		}

		m_windowFrameBuffer.unbind();

		m_batchRenders.clear();

		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}

#if USE_IMGUI
	void Renderer3D::imGuiRender()
	{
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

				const bool focused{ ImGui::IsWindowFocused()  };

				if (focused)
					glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				else
					glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 

				Simulation::m_camera->setActive(focused);
				//viewportSelected = ImGui::IsWindowFocused();

				ImVec2 RegionSize = ImGui::GetContentRegionAvail();

				if ((RegionSize.x != m_windowSize.x) || (RegionSize.y != m_windowSize.y))
				{
					// Always a good idea, when debugging at least, to check for GL errors
		/*			Helpers::CheckForGLError();*/
					m_windowSize.x = RegionSize.x;
					m_windowSize.y = RegionSize.y;

					m_windowFrameBuffer.resize(m_windowSize);

					// Always a good idea, when debugging at least, to check for GL errors
					Helpers::CheckForGLError();
				}
				//ImVec2 windowSize = ImVec2(ImGui::GetCursorScreenPos().x + 1600 / 2, ImGui::GetCursorScreenPos().y + 900 / 2);
				//ImVec2 windowSize = ImVec2(ImGui::GetItemRectMin().x + ImGui::GetCursorScreenPos().x, ImGui::GetItemRectMin().y + ImGui::GetCursorScreenPos().y);
				ImGui::Image((ImTextureID)m_windowFrameBuffer.getColourAttachment(), ImVec2(m_windowSize.x, m_windowSize.y), ImVec2(0, 1), ImVec2(1, 0));
			}
			ImGui::End();
		}

		if ((glfwGetKey(s_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) /*|| (glfwGetKey(s_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)*/)
		{
			Simulation::m_camera->setActive(false);
			ImGui::SetWindowFocus();
			glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		}

		static bool shelfOpen{ true };
		ImGui::Begin("Shelf", &shelfOpen, ImGuiWindowFlags_MenuBar);
		{
			static bool showMaterials{ true };
			static bool showAlbedo{ true };
			static bool showNormal{ true };
			static bool showEmission{ true };

			static std::array<bool, static_cast<int>(TNAP::ETextureType::eCount)> headerOpen;
			headerOpen.fill(true);
			static std::array<std::string, headerOpen.size()> textureNames { "Albedo", "Normal", "Metallic", "Roughness", "AO", "Emission" };

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Create"))
				{
					if (ImGui::BeginMenu("Texture"))
					{
						static std::string textureFilePath{ "" };
						static TNAP::ETextureType textureType{ TNAP::ETextureType::eAlbedo };

						ImGui::InputText("Filepath", &textureFilePath);

						if (ImGui::BeginCombo("Texture Type", textureNames.at(static_cast<int>(textureType)).c_str()))
						{
							for (int i = 0; i < textureNames.size(); i++)
							{
								if (ImGui::Selectable(textureNames.at(i).c_str()))
									textureType = static_cast<TNAP::ETextureType>(i);
							}
							ImGui::EndCombo();
						}

						if (ImGui::MenuItem("Create"))
						{
							if (!textureFilePath.empty())
							{
								loadTexture(textureType, textureFilePath);
								textureFilePath = "";
							}
						}

						ImGui::EndMenu();
					}

					static std::array<std::string, static_cast<int>(TNAP::EMaterialType::eCount)> materialNames{ "Unlit", "UnlitTexture", "PBR"};

					if (ImGui::BeginMenu("Material"))
					{
						static std::string name{ "" };
						static TNAP::EMaterialType materialType{ TNAP::EMaterialType::eUnlit };

						ImGui::InputText("Name", &name);

						if (ImGui::BeginCombo("Material Type", materialNames.at(static_cast<int>(materialType)).c_str()))
						{
							for (int i = 0; i < materialNames.size(); i++)
							{
								if (ImGui::Selectable(materialNames.at(i).c_str()))
									materialType = static_cast<TNAP::EMaterialType>(i);
							}
							ImGui::EndCombo();
						}

						if (ImGui::MenuItem("Create"))
						{
							if (!name.empty())
							{
								if (std::string::npos == name.find('_'))
									name += "_";

								createMaterial(name, materialType, true);
								name = "";
							}
						}

						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Show"))
				{
					ImGui::MenuItem("Materials", NULL, &showMaterials);
					
					if (ImGui::BeginMenu("Textures"))
					{
						for (int i = 0; i < textureNames.size(); i++)
							ImGui::MenuItem(textureNames.at(i).c_str(), NULL, &headerOpen.at(i));
						
						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();

				static bool showTextures;
				showTextures = showAlbedo || showNormal || showEmission;

		if (!showTextures && !showMaterials)
		{
			ImGui::End();
			return;
		}

				static int iconSize{ 32 };
				ImGui::SliderInt("Icon Size", &iconSize, 32, 256);

				if (iconSize <= 0)
					iconSize = 1;

				if (ImGui::CollapsingHeader("Textures", &showTextures))
				{
					
					ImVec2 size{ ImGui::GetContentRegionAvail() };

					int amount{ static_cast<int>(size.x) / iconSize };
					amount = std::max(amount, 1);
					ImGui::Spacing();

					for (int i = 0; i < textureNames.size(); i++)
					{
						const std::vector<std::pair<std::unique_ptr<Helpers::ImageLoader>, GLuint>>& textures{ m_textures.at(i) };
						if (ImGui::CollapsingHeader(textureNames.at(i).c_str(), &headerOpen.at(i)))
						{
							for (int j = 0; j < textures.size(); j++)
							{
								if (j % amount != 0)
									ImGui::SameLine();

								ImGui::ImageButton((ImTextureID)textures[j].second, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0), 1);
								if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
								{
									ImGui::SetDragDropPayload("TEXTURE_CELL", &std::pair<ETextureType, size_t>(static_cast<ETextureType>(i), j), sizeof(std::pair<ETextureType, size_t>));
									ImGui::Image((ImTextureID)textures[j].second, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
									ImGui::Text(("Texture Type: " + std::to_string(i)).c_str());
									ImGui::Text(("Texture Handle: " + std::to_string(j)).c_str());
									ImGui::EndDragDropSource();
								}
							}
						}
					}
				}
				
				ImGui::Spacing();
				ImGui::Spacing();
				if (ImGui::CollapsingHeader("Materials", &headerOpen.at(0)))
				{
					if (ImGui::Button("Save Materials"))
					{
						std::ofstream outputFile;

						outputFile.open("Data/SaveLoad/Materials.csv");

						for (const std::unique_ptr<Material>& mat : m_materials)
						{
							mat->saveData(outputFile, m_programs[mat->getProgramHandle()].m_name);
							outputFile << std::endl;
						}
						outputFile.close();
					}

					ImGui::Columns(3, "materials"); // 3-ways, with border

					ImGui::Separator();
					ImGui::Text("Mat"); ImGui::NextColumn();
					ImGui::Text("Name"); ImGui::NextColumn();
					ImGui::Text("Handle"); ImGui::NextColumn();
					ImGui::Separator();

					for (size_t i = 0; i < m_materials.size(); i++)
					{
						 
						size_t textureGLuint = m_textures.at(static_cast<size_t>(ETextureType::eAlbedo)).at(m_mapTextures.at(ETextureType::eAlbedo).at("Editor/MatTexture.png")).second;

						ImGui::PushID(i);
						ImGui::ImageButton((ImTextureID)textureGLuint, ImVec2(16, 16), ImVec2(0, 1), ImVec2(1, 0), 1);
						ImGui::PopID();

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("MATERIAL_CELL", &i, sizeof(i));
							ImGui::Image((ImTextureID)textureGLuint, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
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

				ImGui::Spacing();
				ImGui::Spacing();
				static bool modelsOpen{ true };
				if (ImGui::CollapsingHeader("Models", &modelsOpen))
				{
					ImGui::Columns(5, "models"); // 5-ways, with border
					ImGui::Separator();
					ImGui::Text("Model"); ImGui::NextColumn();
					ImGui::Text("Path"); ImGui::NextColumn();
					ImGui::Text("Model Handle"); ImGui::NextColumn();
					ImGui::Text("Mesh Count"); ImGui::NextColumn();
					ImGui::Text("Material Count"); ImGui::NextColumn();
					ImGui::Separator();

					for (const auto& mapModel : m_mapModels)
					{
						size_t textureGLuint = m_textures.at(static_cast<size_t>(ETextureType::eAlbedo)).at(m_mapTextures.at(ETextureType::eAlbedo).at("Editor/ModelTexture.png")).second;

						ImGui::PushID(mapModel.second);
						ImGui::ImageButton((ImTextureID)textureGLuint, ImVec2(16, 16), ImVec2(0, 1), ImVec2(1, 0), 1);
						ImGui::PopID();

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
						{
							ImGui::SetDragDropPayload("MODEL_CELL", &mapModel.second, sizeof(mapModel.second));
							ImGui::Image((ImTextureID)textureGLuint, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
							ImGui::Text(("Model Name: " + mapModel.first).c_str());
							ImGui::Text(("Model Handle: " + std::to_string(mapModel.second)).c_str());
							ImGui::EndDragDropSource();
						}
						ImGui::NextColumn();

						ImGui::Text(mapModel.first.c_str()); ImGui::NextColumn();
						ImGui::Text(std::to_string(mapModel.second).c_str()); ImGui::NextColumn();
						ImGui::Text(std::to_string(m_models.at(mapModel.second).getMeshVector().size()).c_str()); ImGui::NextColumn();
						ImGui::Text(std::to_string(m_models.at(mapModel.second).getUniqueMaterialIndicesCount()).c_str()); ImGui::NextColumn();

						ImGui::Separator();
					}

					ImGui::Columns(1, "models");

				}
			}
		}
		ImGui::End();
	}

	void Renderer3D::imGuiRenderMaterial(const size_t argMaterialHandle)
	{
		if(argMaterialHandle >= 0 && argMaterialHandle < m_materials.size())
			m_materials[argMaterialHandle]->imGuiRender();
	}

#endif
}