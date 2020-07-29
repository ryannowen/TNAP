#pragma once

#include "Message.hpp"
#include <string>

namespace TNAP
{

	class Material;

	class LogMessage : public TNAP::Message
	{
	public:
		enum class ELogType
		{
			eInfo,
			eWarning,
			eError
		};

	public:
		LogMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eLogMessage;
		}

		ELogType m_logType{ ELogType::eInfo };
		std::string m_messageBuffer{ "" };
	};

}