#include "FrameBuffer.hpp"

namespace TNAP {

	FrameBuffer::FrameBuffer()
	{
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(1, &m_colourAttachment);
		glDeleteTextures(1, &m_depthAttachment);
	}

	void FrameBuffer::init()
	{
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glGenTextures(1, &m_colourAttachment);
		glBindTexture(GL_TEXTURE_2D, m_colourAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1600, 900, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colourAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, 1600, 900);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	}

	void FrameBuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::resize(const glm::vec2 argSize)
	{
		if (argSize == m_frameBufferSize)
			return;

		m_frameBufferSize = argSize;

		bind();
		glBindTexture(GL_TEXTURE_2D, m_colourAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, argSize.x, argSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		unbind();

		/*glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, argSize.x, argSize.y);
		glBindTexture(GL_TEXTURE_2D, 0);*/
	}

}