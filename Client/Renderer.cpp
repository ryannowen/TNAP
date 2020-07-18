#include "Renderer.h"
#include "ImageLoader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	glDeleteProgram(m_program);	
	glDeleteBuffers(1, &m_VAO);

	glDeleteFramebuffers(1, &fbo);

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<GLfloat> vertices =
	{
		0.0f,  1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f
	};

	std::vector<GLfloat> verticeColours =
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};
	std::vector<GLuint> elements =
	{
		0, 1, 2,
		0, 3, 1
	};

	GLuint positionsVBO;
	GLuint coloursVBO;

	GLuint elementsVAO;

	glGenBuffers(1, &positionsVBO);

	// Bind the buffer to the context at the GL_ARRAY_BUFFER binding point (target). 
	// This is the first time this buffer is used so this also creates the buffer object.
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);

	// Fill the bound buffer with the vertices, we pass the size in bytes and a pointer to the data
	// the last parameter is a hint to OpenGL that we will not alter the vertices again
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	/// Colour buffer
	/// Generates ID for colours
	glGenBuffers(1, &coloursVBO);
	/// Binds Colour ID to buffer
	glBindBuffer(GL_ARRAY_BUFFER, coloursVBO);
	/// Assign Data to Buffer 
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticeColours.size(), verticeColours.data(), GL_STATIC_DRAW);


	/// Element Buffer
	/// Generates ID for colours
	glGenBuffers(1, &elementsVAO);
	/// Binds Colour ID to buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVAO);
	/// Assign Data to Buffer 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * elements.size(), elements.data(), GL_STATIC_DRAW);



	// Clear binding - not absolutely required but a good idea!
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO); // Create a unique id for a vertex array object

	// Bind it to be current and since first use this also allocates memory for it
	// Note no target binding point as there is only one type of vao
	glBindVertexArray(m_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO); // Bind the vertex buffer to the context (records this action in the VAO)
	glEnableVertexAttribArray(0); // Enable the first attribute in the program (the vertices) to stream to the vertex shader

	// Describe the make up of the vertex stream
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, coloursVBO);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVAO);
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
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Clear Viewport Frame Buffer
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our program. Doing this enables the shaders we attached previously.
	glUseProgram(m_program);

	// Use our vertex shader object. This carries out the buffer binding and enabling of the attribute index recorded before.
	// In effect it replays it
	glBindVertexArray(m_VAO);


	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		ImGui::Begin("Viewport");
		{
			ImGui::Text("Viewport Test");
			ImVec2 windowSize = ImGui::GetWindowSize();
			//ImGui::Image((ImTextureID)textureBuffer, windowSize, ImVec2(0, 0), ImVec2(1, 1));
			ImGui::GetWindowDrawList()->AddImage(
				(void*)textureBuffer,
				ImVec2(ImGui::GetCursorScreenPos()),
				ImVec2(ImGui::GetCursorScreenPos().x + 1600 / 2,
					ImGui::GetCursorScreenPos().y + 900 / 2), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::End();
	}

	// Always a good idea, when debugging at least, to check for GL errors
	Helpers::CheckForGLError();
}
