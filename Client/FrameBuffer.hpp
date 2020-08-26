#pragma once

#include <vector>

#include "ExternalLibraryHeaders.h"
#include "Engine.hpp"
#include "LogMessage.hpp"


namespace TNAP {

	enum class EFrameBufferAttachmentType
	{
		eColour = GL_COLOR_ATTACHMENT0,
		eDepth = GL_DEPTH_ATTACHMENT,
		eStencil = GL_STENCIL_ATTACHMENT,
		eDepthStencil = GL_DEPTH_STENCIL_ATTACHMENT
	};

	struct SAttachmentData
	{
		EFrameBufferAttachmentType m_attachmentType{ EFrameBufferAttachmentType::eColour };
		GLuint m_frameBufferAttachment{ 0 };
		GLuint m_internalFormat{ 0 };
		GLuint m_pixelFormat{ 0 };
		GLuint m_dataType{ 0 };

		SAttachmentData() {}
		SAttachmentData(const EFrameBufferAttachmentType argAttachmentType, const GLuint argInternalFormat, const GLuint argPixelFormat, const GLuint argDataType)
			: m_attachmentType(argAttachmentType), m_internalFormat(argInternalFormat), m_pixelFormat(argPixelFormat), m_dataType(argDataType)
		{
		}
	};

	class FrameBuffer
	{
	private:
		glm::vec2 m_frameBufferSize{ 0, 0 };
		GLuint m_fbo{ 0 };
		GLuint m_numberOfColourAttachments{ 0 };
		std::vector<SAttachmentData> m_attachments;

	public:
		FrameBuffer();
		~FrameBuffer();
		void init();

		inline const bool bind(const bool argCheckStatus = true)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
			if (argCheckStatus && glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				LogMessage message("Framebuffer status not complete!");
				message.m_logType = LogMessage::ELogType::eError;

				getEngine()->sendMessage(&message);

				unbind();
				return false;
			}
			return true;
		}

		inline void unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void resize(const glm::vec2 argSize);

		void addColourAttachment(const glm::vec2& argSize, const GLint argInternalFormat = GL_RGB, const GLint argPixelFormat = GL_RGB, const GLint argDataType = GL_UNSIGNED_BYTE, const GLint argTextureFilter = GL_LINEAR);
		void addDepthAttachment(const glm::vec2& argSize, const GLint argInternalFormat = GL_DEPTH_COMPONENT, const GLint argPixelFormat = GL_DEPTH_COMPONENT, const GLint argDataType = GL_FLOAT);
		//void addStencilAttachment(const glm::vec2& argSize, const GLint argInternalFormat = GL_STENCIL_CO, const GLint argPixelFormat = GL_DEPTH_COMPONENT, const GLint argDataType = GL_FLOAT);
		void addDepthStencilAttachment(const glm::vec2& argSize, const GLint argInternalFormat = GL_DEPTH24_STENCIL8, const GLint argPixelFormat = GL_DEPTH_STENCIL, const GLint argDataType = GL_UNSIGNED_INT_24_8);

		inline const glm::vec2 getSize() const { return m_frameBufferSize; }
		inline const GLuint getFBO() const { return m_fbo; }
		inline const GLuint getAttachmentOfType(const EFrameBufferAttachmentType argAttachmentType, const size_t argTypeIndex = 0) const 
		{
			GLuint firstAttachment{ 0 };
			size_t typeCounter{ 0 };
			for (const SAttachmentData& data : m_attachments)
			{
				if (argAttachmentType != data.m_attachmentType)
					continue;

				if (0 == firstAttachment)
					firstAttachment = data.m_frameBufferAttachment;

				if (argTypeIndex == typeCounter)
					return data.m_frameBufferAttachment;

				typeCounter++;
			}
			if (0 == firstAttachment)
				return m_attachments.at(0).m_frameBufferAttachment;

			return firstAttachment;
		}
	};
}

