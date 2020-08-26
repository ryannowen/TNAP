#include "FrameBuffer.hpp"
#include "Helper.h"

namespace TNAP {

	FrameBuffer::FrameBuffer()
	{
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_fbo);
		for (const SAttachmentData& data : m_attachments)
			glDeleteTextures(1, &data.m_frameBufferAttachment);
	}

	void FrameBuffer::init()
	{
		glGenFramebuffers(1, &m_fbo);
		addColourAttachment(glm::vec2(1));
		//addDepthStencilAttachment(glm::vec2(1));
		Helpers::CheckForGLError();
	}

	void FrameBuffer::resize(const glm::vec2 argSize)
	{
		if (argSize == m_frameBufferSize)
			return;

		m_frameBufferSize = argSize;

		if (!bind())
			return;
		for (const SAttachmentData& data : m_attachments)
		{
			//if (EFrameBufferAttachmentType::eColour != data.m_attachmentType)
			//	continue;
			glBindTexture(GL_TEXTURE_2D, data.m_frameBufferAttachment);
			glTexImage2D(GL_TEXTURE_2D, 0, data.m_internalFormat, static_cast<GLsizei>(argSize.x), static_cast<GLsizei>(argSize.y), 0, data.m_pixelFormat, data.m_dataType, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		unbind();

		/*glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, argSize.x, argSize.y);
		glBindTexture(GL_TEXTURE_2D, 0);*/
	}

	void FrameBuffer::addColourAttachment(const glm::vec2& argSize, const GLint argInternalFormat, const GLint argPixelFormat, const GLint argDataType, const GLint argTextureFilter)
	{
		if (!bind(false))
			return;
		m_attachments.emplace_back(SAttachmentData(EFrameBufferAttachmentType::eColour, argInternalFormat, argPixelFormat, argDataType));

		glGenTextures(1, &m_attachments.back().m_frameBufferAttachment);
		glBindTexture(GL_TEXTURE_2D, m_attachments.back().m_frameBufferAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, argInternalFormat, argSize.x, argSize.y, 0, argPixelFormat, argDataType, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, argTextureFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, argTextureFilter);

		glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(EFrameBufferAttachmentType::eColour)+m_numberOfColourAttachments, GL_TEXTURE_2D, m_attachments.back().m_frameBufferAttachment, 0);
		m_numberOfColourAttachments++;
		unbind();
		Helpers::CheckForGLError();
	}

	void FrameBuffer::addDepthStencilAttachment(const glm::vec2& argSize, const GLint argInternalFormat, const GLint argPixelFormat, const GLint argDataType)
	{
		if (!bind(false))
			return;
		m_attachments.emplace_back(SAttachmentData(EFrameBufferAttachmentType::eDepthStencil, argInternalFormat, argPixelFormat, argDataType));
		//glCreateTextures(GL_TEXTURE_2D, 1, &m_attachments.back().m_frameBufferAttachment);

		glGenTextures(1, &m_attachments.back().m_frameBufferAttachment);
		glBindTexture(GL_TEXTURE_2D, m_attachments.back().m_frameBufferAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, argInternalFormat, argSize.x, argSize.y, 0, argPixelFormat, argDataType, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_ALPHA);

		glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(EFrameBufferAttachmentType::eDepthStencil), GL_TEXTURE_2D, m_attachments.back().m_frameBufferAttachment, 0);
		unbind();
		Helpers::CheckForGLError();
	}

	void FrameBuffer::addDepthAttachment(const glm::vec2& argSize, const GLint argInternalFormat, const GLint argPixelFormat, const GLint argDataType)
	{
		if (!bind(false))
			return;
		m_attachments.emplace_back(SAttachmentData(EFrameBufferAttachmentType::eDepth, argInternalFormat, argPixelFormat, argDataType));
		glCreateTextures(GL_TEXTURE_2D, 1, &m_attachments.back().m_frameBufferAttachment);
		glBindTexture(GL_TEXTURE_2D, m_attachments.back().m_frameBufferAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, argInternalFormat, argSize.x, argSize.y, 0, argPixelFormat, argDataType, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<GLenum>(EFrameBufferAttachmentType::eDepth), GL_TEXTURE_2D, m_attachments.back().m_frameBufferAttachment, 0);
		unbind();
		Helpers::CheckForGLError();
	}

}