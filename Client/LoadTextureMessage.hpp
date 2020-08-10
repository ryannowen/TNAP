#pragma once

#include "Message.hpp"

#include <string>

#include "STextureData.hpp"

namespace TNAP {

	enum class ETextureType;

	class LoadTextureMessage : public TNAP::Message
	{
	public:
		LoadTextureMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eLoadTextureMessage;
		}

		LoadTextureMessage(const TNAP::ETextureType argTextureType, const std::string& argFilePath, const std::string& argName = "")
			: TNAP::Message(),
			m_loadInfo(argTextureType, argFilePath), m_textureName(argName)
		{
			m_messageType = EMessageType::eLoadTextureMessage;
		}

		std::string m_textureName{ "" };
		std::pair<TNAP::ETextureType, std::string> m_loadInfo;

		bool m_loadedSuccessfully{ false };
		STextureData* m_textureData{ nullptr };
		size_t m_textureHandle{ 0 };
	};

}