#pragma once

#include "Message.hpp"

#include "ExternalLibraryHeaders.h"
#include "Renderer3D.hpp"
#include "ImageLoader.h"

namespace TNAP
{

	class GetTextureMessage : public TNAP::Message
	{
	public:
		GetTextureMessage(const std::pair<ETextureType, size_t> argTextureInfo)
			: TNAP::Message(),
			m_textureInfo(argTextureInfo)
		{
			m_messageType = EMessageType::eGetTextureMessage;
		}

		std::pair<ETextureType, size_t> m_textureInfo{ ETextureType::eAlbedo, 0 };
		Helpers::ImageLoader* m_textureData{ nullptr };
		GLuint m_textureBinding{ 0 };
	};

}