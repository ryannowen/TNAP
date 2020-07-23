#include "Renderer.h"
#include "ImageLoader.h"

int Entity::treeIndex{ 0 };
Entity* Entity::selected{ nullptr };


// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	
	glDeleteBuffers(1, &m_VAO);

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &textureBuffer);
	glDeleteTextures(1, &m_DepthAttachment);

}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return !Helpers::CheckForGLError();
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1600, 900, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, 1600, 900);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);

	windowSize.x = viewportSize[2];
	windowSize.y = viewportSize[3];

	/// <summary>
	/// Creates Textures for IMGUI
	/// </summary>
	/// <returns></returns>
	for (int i = 0; i < textureNames.size(); i++)
	{
		contextTextures.push_back(0);
		glGenTextures(1, &contextTextures.back());
		glBindTexture(GL_TEXTURE_2D, contextTextures.back());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::string textName(textureNames[i]);
		Helpers::ImageLoader image = Helpers::ImageLoader();
		if (!image.Load("Data\\Textures\\" + textName))
			std::cerr << textName << "does not exist Returning missing texture" << std::endl;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Width(), image.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetData());
	}

	Helpers::ModelLoader model;
	if (!model.LoadFromFile("Data\\Models\\AssaultRifleModel.fbx"))
	{
		std::cout << "test";
	}

	std::vector<glm::vec3> vertices{ model.GetMeshVector()[0].vertices };
	std::vector<glm::vec2> uvCoord{ model.GetMeshVector()[0].uvCoords };
	std::vector<GLuint> elements{ model.GetMeshVector()[0].elements };

	{
		//std::vector<GLfloat> vertices =
		//{
		//	// Front 
		//	-10.0f, -10.0f, 10.0f, /// 0
		//	10.0f, 10.0f, 10.0f, /// 1
		//	-10.0f, 10.0f, 10.0f, /// 2
		//	10.0f, -10.0f, 10.0f, /// 3

		//	 //Back
		//	-10.0f, -10.0f, -10.0f, /// 4
		//	10.0f, -10.0f, -10.0f, /// 5
		//	10.0f, 10.0f, -10.0f, /// 6
		//	-10.0f, 10.0f, -10.0f /// 7

		//};

		//std::vector<GLfloat> verticeColours =
		//{

		//	// Front
		//	1.0f, 0.0f, 0.0f,
		//	0.0f, 1.0f, 0.0f,
		//	0.0f, 0.0f, 1.0f,
		//	1.0f, 1.0f, 0.0f,

		//	// Back
		//	0.0f, 1.0f, 1.0f,
		//	1.0f, 1.0f, 1.0f,
		//	0.3f, 0.3f, 1.0f,
		//	0.7f, 0.7f, 0.7f


		//};

		//std::vector <GLfloat> uvCoord = 
		//{
		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, //

		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, //
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, //

		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		//	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		//};

		//std::vector<GLuint> elements =
		//{
		//	0, 1, 2, 0, 3, 1, /// Front /
		//	4, 2, 7, 4, 0, 2, /// Left /
		//	3, 6, 1, 3, 5, 6, /// Right
		//	2, 6, 7, 2, 1, 6, /// Top
		//	4, 5, 0, 5, 3, 0, /// Bottom
		//	5, 7, 6, 5, 4, 7 /// Back
		//};
	}
	m_numElements = elements.size();

	GLuint positionsVBO;
	GLuint coloursVBO;
	GLuint uvsVBO;

	GLuint elementsVAO;

	glGenBuffers(1, &positionsVBO);

	// Bind the buffer to the context at the GL_ARRAY_BUFFER binding point (target). 
	// This is the first time this buffer is used so this also creates the buffer object.
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);

	// Fill the bound buffer with the vertices, we pass the size in bytes and a pointer to the data
	// the last parameter is a hint to OpenGL that we will not alter the vertices again
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	/// Colour buffer
	/*glGenBuffers(1, &coloursVBO);
	glBindBuffer(GL_ARRAY_BUFFER, coloursVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticeColours.size(), verticeColours.data(), GL_STATIC_DRAW);*/

	/// UV buffer
	glGenBuffers(1, &uvsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* uvCoord.size(), uvCoord.data(), GL_STATIC_DRAW);

	/// Element Buffer
	glGenBuffers(1, &elementsVAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVAO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);



	// Clear binding - not absolutely required but a good idea!
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO); // Create a unique id for a vertex array object

	// Bind it to be current and since first use this also allocates memory for it
	// Note no target binding point as there is only one type of vao
	glBindVertexArray(m_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO); // Bind the vertex buffer to the context (records this action in the VAO)
	glEnableVertexAttribArray(0); // Enable the first attribute in the program (the vertices) to stream to the vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//glBindBuffer(GL_ARRAY_BUFFER, coloursVBO);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVAO);
	//glEnableVertexAttribArray(2);
	glBindVertexArray(0);


	std::vector<glm::vec3> vertices2{ model.GetMeshVector()[1].vertices };
	std::vector<glm::vec2> uvCoord2{ model.GetMeshVector()[1].uvCoords };
	std::vector<GLuint> elements2{ model.GetMeshVector()[1].elements };

			
	m_numElements2 = elements2.size();

	GLuint positionsVBO2;
	GLuint uvsVBO2;

	GLuint elementsVAO2;

	glGenBuffers(1, &positionsVBO2);

	// Bind the buffer to the context at the GL_ARRAY_BUFFER binding point (target). 
	// This is the first time this buffer is used so this also creates the buffer object.
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO2);

	// Fill the bound buffer with the vertices, we pass the size in bytes and a pointer to the data
	// the last parameter is a hint to OpenGL that we will not alter the vertices again
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);

	/// UV buffer
	glGenBuffers(1, &uvsVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvCoord2.size(), uvCoord2.data(), GL_STATIC_DRAW);

	/// Element Buffer
	glGenBuffers(1, &elementsVAO2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVAO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements2.size(), elements2.data(), GL_STATIC_DRAW);



	// Clear binding - not absolutely required but a good idea!
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO2); // Create a unique id for a vertex array object

	// Bind it to be current and since first use this also allocates memory for it
	// Note no target binding point as there is only one type of vao
	glBindVertexArray(m_VAO2);


	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO2); // Bind the vertex buffer to the context (records this action in the VAO)
	glEnableVertexAttribArray(0); // Enable the first attribute in the program (the vertices) to stream to the vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO2);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVAO2);
	glEnableVertexAttribArray(2);

	
	// Good idea to check for an error now:	
	Helpers::CheckForGLError();

	// Clear VAO binding
	glBindVertexArray(0);

	return true;
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{		
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Uncomment to render in wireframe (can be useful when debugging)
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Clear Default Frame Buffer
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if USEIMGUI
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, windowSize.x, windowSize.y);

	// Clear Viewport Frame Buffer
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen) 
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			//if (ImGui::MenuItem("Exit"));
			ImGui::EndMenu();
		}
		
		ImGui::Indent(100);
		ImGui::Text("This is a label on the menu bar which is indented by 100");

		ImGui::EndMenuBar();
	}
	static bool shelfOpen{ true };
	ImGui::Begin("Shelf", &shelfOpen, ImGuiWindowFlags_MenuBar);
	{    
		static bool showMaterials{ true };
		static bool showAlbedo{ true };
		static bool showNormal{ true };
		static bool showEmission{ true };
		if (ImGui::BeginMenuBar())
		{
			if(ImGui::BeginMenu("Show"))
			{
				ImGui::MenuItem("Materials", NULL, &showMaterials);

				if (ImGui::BeginMenu("Textures"))
				{
					ImGui::MenuItem("Albedo", NULL, &showAlbedo);
					ImGui::MenuItem("Normals", NULL, &showNormal);
					ImGui::MenuItem("Emissives", NULL, &showEmission);
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImVec2 size{ ImGui::GetWindowSize() };
		if (showAlbedo || showNormal || showEmission) 
		{
			if (ImGui::CollapsingHeader("Textures"))
			{
				static int texturesSize{ 64 };

				ImGui::SliderInt("TextureSize", &texturesSize, 32, 256);

				if (showAlbedo)
				{
					if (ImGui::CollapsingHeader("Albedo"))
					{
						int amount{ static_cast<int>(size.x) / texturesSize };
						amount = std::max(amount, 1);
						for (int i = 0; i < contextTextures.size(); i++)
						{
							if (i % amount != 0)
								ImGui::SameLine();

							ImGui::ImageButton((ImTextureID)contextTextures[i], ImVec2(texturesSize, texturesSize), ImVec2(0, 0), ImVec2(1, 1), 1);
							if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
							{

								ImGui::SetDragDropPayload("TEXTURE_CELL", &contextTextures[i], sizeof(GLuint));
								ImGui::Image((ImTextureID)contextTextures[i], ImVec2(64, 64));
								ImGui::EndDragDropSource();
							}
						}
					}
				}

				if (showNormal)
				{
					if (ImGui::CollapsingHeader("Normal"))
					{
					}
				}

				if (showEmission)
				{
					if (ImGui::CollapsingHeader("Emissives"))
					{
					}
				}
			}
		}
	}
	ImGui::End();

	static bool showHierarchy{ true };
	ImGui::Begin("Hierarchy", &showHierarchy, ImGuiWindowFlags_MenuBar);
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Create"))
			{
				if(ImGui::MenuItem("Entity"))
				{
					Entity::selected->children.push_back(std::make_shared<Entity>("New Entity!", std::vector<std::shared_ptr<Entity>>()));
				}

				if (ImGui::MenuItem("Player"))
				{
					Entity::selected->children.push_back(std::make_shared<Player>("New Player!", std::vector<std::shared_ptr<Entity>>()));
				}

				if (ImGui::MenuItem("Mesh"))
				{
					Entity::selected->children.push_back(std::make_shared<MeshTest>("New Mesh!", std::vector<std::shared_ptr<Entity>>()));
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::TextColored(ImVec4(0.9, 0.49, 0.17, 1), "Example Scene Name");

		Entity::treeIndex = 0;

		for (int i = 0; i < entities.size(); i++)
			entities[i]->ImGuiDrawTree();
	}
	ImGui::End();

	ImGui::Begin("Entity Properties");
	{
		if (nullptr != Entity::selected)
			Entity::selected->ImGuiDrawInformation();
	}
	ImGui::End();

	ImGui::Begin("Scene Properties");
	{
		ImGui::TextColored(ImVec4(0.9, 0.49, 0.17, 1), "Example Scene Name");

		static bool useLights{ false };
		ImGui::Checkbox("Use Lights", &useLights);

		if (useLights)
			ImGui::ShowDemoWindow(&useLights);

	}
	ImGui::End();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		static bool viewportOpen = true;
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_None;
		//flags |= ImGuiWindowFlags_NoMove;
		//flags |= ImGuiWindowFlags_NoResize;
		flags |= ImGuiWindowFlags_NoScrollbar;
		flags |= ImGuiWindowFlags_NoScrollWithMouse;


		ImGui::Begin("Viewport", &viewportOpen, flags);

		//if(ImGui::IsWindowHovered())
		viewportSelected = ImGui::IsWindowFocused();
		/*else
			viewportSelected = false;*/

			//ImVec2 windowSize = ImGui::GetWindowSize();
			//ImVec2 windowSize(1600, 900);


		ImVec2 RegionSize = ImGui::GetContentRegionAvail();

		if ((RegionSize.x != windowSize.x) || (RegionSize.y != windowSize.y))
		{
			// Always a good idea, when debugging at least, to check for GL errors
			Helpers::CheckForGLError();
			windowSize = RegionSize;

			glBindTexture(GL_TEXTURE_2D, textureBuffer);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSize.x, windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

			glBindTexture(GL_TEXTURE_2D, 0);

			// Always a good idea, when debugging at least, to check for GL errors
			Helpers::CheckForGLError();
		}
		//ImVec2 windowSize = ImVec2(ImGui::GetCursorScreenPos().x + 1600 / 2, ImGui::GetCursorScreenPos().y + 900 / 2);
		//ImVec2 windowSize = ImVec2(ImGui::GetItemRectMin().x + ImGui::GetCursorScreenPos().x, ImGui::GetItemRectMin().y + ImGui::GetCursorScreenPos().y);
		ImGui::Image((ImTextureID)textureBuffer, windowSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}
#endif

	// Compute viewport and projection matrix

	const float aspect_ratio = (float)windowSize.x / (float)windowSize.y;
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 400.0f);

	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);

	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));

	glm::mat4 model_xform = glm::mat4(1);

	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, matTexture);

	/// Sends Texture Sample to shader
	GLuint texSample_id = glGetUniformLocation(m_program, "texSample");
	glUniform1i(texSample_id, 0);

	GLuint colourUni_id = glGetUniformLocation(m_program, "colourUni");
	glUniform3f(colourUni_id, 1, 0, 0);

	// Use our vertex shader object. This carries out the buffer binding and enabling of the attribute index recorded before.
	// In effect it replays it
	glBindVertexArray(m_VAO);


	glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, (void*)0);

	glUniform3f(colourUni_id, 0, 1, 0);

	glBindVertexArray(0);


	glBindVertexArray(m_VAO2);


	glDrawElements(GL_TRIANGLES, m_numElements2, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);

#if USEIMGUI

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::End();
#endif

	// Always a good idea, when debugging at least, to check for GL errors
	Helpers::CheckForGLError();
}
