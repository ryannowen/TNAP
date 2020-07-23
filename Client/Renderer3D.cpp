#include "Renderer3D.hpp"

namespace TNAP {
	GLFWwindow* Renderer3D::s_window{ nullptr };

	void Renderer3D::CreateProgram(const std::string& argVertexFilePath, const std::string& argFragmentFilePath)
	{
	}

	Renderer3D::Renderer3D()
	{
	}

	Renderer3D::~Renderer3D()
	{
		glDeleteProgram(m_program);

		glfwDestroyWindow(s_window);
		glfwTerminate();
	}

	void Renderer3D::init()
	{
		// Use the helper function to set up GLFW, GLEW and OpenGL
		s_window = Helpers::CreateGLFWWindow(1600, 900, "TNAP");
		if (!s_window)
			return;


		m_program = glCreateProgram();

		// Load and create vertex and fragment shaders
		GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
		GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
		if (vertex_shader == 0 || fragment_shader == 0)
			return;

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
			return;

		Helpers::CheckForGLError();

		glfwSetInputMode(s_window, GLFW_STICKY_KEYS, GLFW_TRUE);

	}

	void Renderer3D::update()
	{
		render();
	}

	void Renderer3D::sendMessage()
	{
	}

	void Renderer3D::loadModel(const std::string& argFilePath)
	{
	}

	void Renderer3D::loadTexture(const std::string& argFilePath)
	{
	}

	void Renderer3D::loadMaterials(const std::string& argFilePath)
	{
		// TODO
		// Load materials from file
		// Check if material shaders already exist
		// assign programHandle to material

	}

	void Renderer3D::render()
	{
		// Configure pipeline settings
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// Uncomment to render in wireframe (can be useful when debugging)
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Clear buffers from previous frame
		glClearColor(0.2f, 0.2f, 0.2f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Always a good idea, when debugging at least, to check for GL errors
		Helpers::CheckForGLError();
	}

#if USE_IMGUI
	void Renderer3D::imGuiRender()
	{
	}
	void Renderer3D::imGuiRenderMaterial()
	{
	}
#endif
}