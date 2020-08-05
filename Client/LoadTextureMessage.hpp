#pragma once

#include "Message.hpp"

#include <string>

#include "STextureData.hpp"

namespace TNAP {

	enum class ETextureType;

	class LoadTextureMessage : public TNAP::Message
	{
	public:
		LoadTextureMessage(const TNAP::ETextureType argTextureType, const std::string& argFilePath)
			: TNAP::Message(),
			m_loadInfo(argTextureType, argFilePath)
		{
			m_messageType = EMessageType::eLoadTextureMessage;
		}

		std::pair<TNAP::ETextureType, std::string> m_loadInfo;

		bool m_loadedSuccessfully{ false };
		STextureData* m_textureData;
		size_t m_textureHandle;
	};

}