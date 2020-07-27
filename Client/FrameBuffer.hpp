#pragma once

#include "ExternalLibraryHeaders.h"

namespace TNAP {

	class FrameBuffer
	{
	private:
		glm::vec2 m_frameBufferSize{ 0, 0 };
		GLuint m_fbo{ 0 };
		GLuint m_colourAttachment{ 0 };
		GLuint m_depthAttachment{ 0 };
	public:
		FrameBuffer();
		~FrameBuffer();
		void init();
		void bind();
		void unbind();
		void resize(const glm::vec2 argSize);

		inline const glm::vec2 getSize() const { return m_frameBufferSize; }
		inline const GLuint getFBO() const { return m_fbo; }
		inline const GLuint getColourAttachment() const { return m_colourAttachment; }
	};
}

