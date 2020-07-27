#include "Renderer3D.hpp"

#include <array>

#include "ImGuiInclude.hpp"
#include "ExternalLibraryHeaders.h"
#include "Mesh.h"
#include "Simulation.h"
#include "GetTextureMessage.hpp"

namespace TNAP {
	GLFWwindow* Renderer3D::s_window{ nullptr };

	void Renderer3D::CreateProgram(const std::string& argVertexFilePath, const std::string& argFragmentFilePath)
	{
		
	}

	Renderer3D::Renderer3D()
	{
		m_textures.resize(static_cast<int>(ETextureType::eCount));
	}

	Renderer3D::~Renderer3D()
	{
		for(const GLuint program : m_programs)
			glDeleteProgram(program);


		glfwDestroyWindow(s_window);
		glfwTerminate();
	}

	void Renderer3D::init()
	{
		// Use the helper function to set up GLFW, GLEW and OpenGL
		s_window = Helpers::CreateGLFWWindow(m_windowSize.x, m_windowSize.y, "TNAP");
		if (!s_window)
			return;


		loadMaterials("Data\\Materials");

		m_windowFrameBuffer.init();
		m_windowFrameBuffer.resize(glm::vec2( m_windowSize.x, m_windowSize.y));
		Helpers::CheckForGLError();

		glfwSetInputMode(s_window, GLFW_STICKY_KEYS, GLFW_TRUE);

		loadModel("AssaultRifleModel.fbx"); // TEMP
		loadTexture(TNAP::ETextureType::eAO, "aqua_pig_2K.png");
		loadTexture(TNAP::ETextureType::eAlbedo, "MissingTexture.jpg");
	}

	void Renderer3D::update()
	{
		render();
	}

	void Renderer3D::sendMessage(TNAP::Message* const argMessage)
	{
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

		default:
			break;
		}

	}

	void Renderer3D::loadModel(const std::string& argFilePath)
	{
		if (m_mapModels.find(argFilePath) != m_mapModels.end())
		{
			// TODO Log Model already loaded
		}

		m_models.emplace_back(TNAP::Model());

		m_mapModels.insert({ argFilePath, m_models.size() - 1 });

		m_models.back().loadFromFile("Data\\Models\\" + argFilePath);
		// TODO Log Model loaded


		std::vector<glm::mat4> transforms
		{
			glm::mat4(1)
		};

		for (int i = 0; i < 13000; i++)
		{
			glm::mat4 newTransform(1);

			newTransform = glm::translate(newTransform, glm::vec3(rand() % 1000, rand() % 1000, rand() % 1000));
			newTransform = glm::rotate(newTransform, static_cast<float>(rand() % 360), glm::vec3(1, 0, 0));
			newTransform = glm::rotate(newTransform, static_cast<float>(rand() % 360), glm::vec3(0, 1, 0));
			newTransform = glm::rotate(newTransform, static_cast<float>(rand() % 360), glm::vec3(0, 0, 1));

			transforms.push_back(newTransform);
		}

		std::vector<size_t> materialHandles
		{
			0
		};

		std::vector<std::pair<std::vector<glm::mat4>, std::vector<size_t>>> modelTemp
		{
			{transforms, materialHandles}
		};

		m_batchRenders.insert({ 0, modelTemp });
	}

	void Renderer3D::loadTexture(const TNAP::ETextureType argType, const std::string& argFilePath)
	{
		//const auto& textMap{  };
		if (m_mapTextures.find(argType) == m_mapTextures.end())
		{
			m_mapTextures.insert({ argType, std::unordered_map<std::string, size_t>() });
		}

		if (m_mapTextures.at(argType).find(argFilePath) != m_mapTextures.at(argType).end())
		{
			// TODO Log Texture already created
		}

		std::unique_ptr<Helpers::ImageLoader> texture{ std::make_unique<Helpers::ImageLoader>() };

		if (!texture->Load("Data\\Textures\\" + argFilePath))
		{
			// TODO Log Missing texture
			return;
		}

		// TODO Log Loaded Texture


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
		m_mapTextures.at(argType).insert({ argFilePath, m_textures.at(static_cast<int>(argType)).size() - 1 });
	}

	void Renderer3D::loadMaterials(const std::string& argFilePath)
	{
		// TODO
		// Load materials from file
		// Check if material shaders already exist
		// assign programHandle to material

		// Load material here

		// Load shaders if is not created
		
		if (m_mapPrograms.find("Unlit") == m_mapPrograms.end())
		{
			GLuint program = glCreateProgram();

			// Load and create vertex and fragment shaders
			GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/unlit_vertex_shader.glsl") };
			GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/unlit_fragment_shader.glsl") };

			if (0 == vertex_shader || 0 == fragment_shader)
			{
				glDeleteProgram(program);
				return;
			}

			// Attach the vertex shader & fragment shader to this program (copies them)
			glAttachShader(program, vertex_shader);
			glAttachShader(program, fragment_shader);

			// Done with the originals of these as we have made copies
			glDeleteShader(vertex_shader);
			glDeleteShader(fragment_shader);

			// Link the shaders, checking for errors
			if (!Helpers::LinkProgramShaders(program))
				return;

			m_programs.emplace_back(program);
			m_mapPrograms.insert({ "Unlit", m_programs.size() - 1 });
		}
		

		if (m_mapMaterials.find("DefaultMaterial") != m_mapMaterials.end())
		{
			// material already Created 
			// continue;
		}

		m_materials.emplace_back(std::make_unique<Material>());
		m_mapMaterials.insert({ "DefaultMaterial", m_materials.size() - 1 });

		m_materials.back()->m_programHandle = m_mapPrograms.at("Unlit");

		// Set other Data
		/*switch (EMaterialType)
		{
		case TNAP::EMaterialType::eUnlit:
			break;
		case TNAP::EMaterialType::eUnlitTexture:
			break;
		case TNAP::EMaterialType::ePBR:
			break;
		default:
			break;
		}*/

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
		
		/// Uses Shaders from our program
		Material* mat{ m_materials[m_mapMaterials.at("DefaultMaterial")].get() };

		GLuint program{ m_programs[mat->getProgramHandle()] };

		if (program != m_currentProgram)
		{
			glUseProgram(program);
			m_currentProgram = program;
		}

		m_windowFrameBuffer.bind();
		glViewport(0, 0, m_windowFrameBuffer.getSize().x, m_windowFrameBuffer.getSize().y);

		// Clear FRAME buffers from previous frame
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		/// Create camera ID and then Sends camera forward direction data to shader as Uniform
		GLuint camera_direcion_id = glGetUniformLocation(program, "camera_direction");
		glUniform3fv(camera_direcion_id, 1, glm::value_ptr(Simulation::m_camera->GetLookVector()));


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


		/// Create combined xform ID and then Sends Combined Xform data to shader as Uniform
		GLuint combined_xform_id = glGetUniformLocation(program, "combined_xform");
		glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

		for (const auto& modelBatch : m_batchRenders)
		{
			const Model* const model{ &m_models.at(modelBatch.first) };

			// Transforms, MaterialHandles
			for (const std::pair<std::vector<glm::mat4>, std::vector<size_t>>& batch : modelBatch.second)
			{
				if (batch.first.size() <= 0)
					continue;

				const std::vector<std::unique_ptr<Helpers::Mesh>>& meshes{ model->getMeshVector() };
				
				for (int i = 0; i < meshes.size(); i++)
				{
					glBindVertexArray(meshes[i]->VAO);

					if (i >= batch.second.size())
						m_materials[0]->sendShaderData();
					else
						m_materials[batch.second[i]]->sendShaderData();

					if (batch.first.size() > 1) // Batch Draw
					{
						GLuint batchRenderingBuffer{ 0 };
						glGenBuffers(1, &batchRenderingBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, batchRenderingBuffer);
						glBufferData(GL_ARRAY_BUFFER, batch.first.size() * sizeof(glm::mat4), &batch.first.at(0), GL_STATIC_DRAW);

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

						glDrawElementsInstanced(GL_TRIANGLES, meshes[i]->elements.size(), GL_UNSIGNED_INT, 0, batch.first.size());

						glDeleteBuffers(1, &batchRenderingBuffer);

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


		/*for (TNAP::Model& model : m_models)
		{
			for (std::unique_ptr<Helpers::Mesh>& mesh : model.getMeshVector())
			{
				glBindVertexArray(mesh->VAO);

				glDrawElements(GL_TRIANGLES, mesh->elements.size(), GL_UNSIGNED_INT, (void*)0);
				
				glBindVertexArray(0);

			}
		}*/

		m_windowFrameBuffer.unbind();

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

			// plus 1 since material isn't a texture type

			static std::array<bool, static_cast<int>(ETextureType::eCount) + 1> headerOpen;
			headerOpen.fill(true);
			static std::array<std::string, headerOpen.size()> headerTitle { "Materials", "Albedo", "eNormal", "Metallic", "Roughness", "AO", "Emission" };

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Show"))
				{
					ImGui::MenuItem("Materials", NULL, &headerOpen.at(0));

					if (ImGui::BeginMenu("Textures"))
					{
						for (int i = 1; i < headerTitle.size(); i++)
							ImGui::MenuItem(headerTitle.at(i).c_str(), NULL, &headerOpen.at(i));
						
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

					// Start at 1 since material is 0
					for (int i = 1; i < headerTitle.size(); i++)
					{
						const std::vector<std::pair<std::unique_ptr<Helpers::ImageLoader>, GLuint>>& textures{ m_textures.at(i - 1) };
						if (ImGui::CollapsingHeader(headerTitle.at(i).c_str(), &headerOpen.at(i)))
						{
							for (int i = 0; i < textures.size(); i++)
							{
								if (i % amount != 0)
									ImGui::SameLine();

								ImGui::ImageButton((ImTextureID)textures[i].second, ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1), 1);
								if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
								{
									ImGui::SetDragDropPayload("TEXTURE_CELL", &textures[i].second, sizeof(GLuint));
									ImGui::Image((ImTextureID)textures[i].second, ImVec2(64, 64));
									ImGui::EndDragDropSource();
								}
							}
						}
					}
				}
				
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::CollapsingHeader("Materials", &headerOpen.at(0));
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