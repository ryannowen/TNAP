#pragma once

#include "ExternalLibraryHeaders.h"

namespace TNAP {

	class FrameBuffer
	{
	private:
		glm::vec2 m_frameBufferSize{ 0, 0 };
		GLuint m_colourAttachment{ 0 };

	public:
		FrameBuffer();
		~FrameBuffer();
		void init();
		void bind();
		void unbind();
		void resize();

	};
}

