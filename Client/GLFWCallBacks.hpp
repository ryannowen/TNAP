#pragma once

#include "Message.hpp"

#include <vector>
#include <string>

namespace TNAP {

	class GLFWDropCallBackMessage : public TNAP::Message
	{
	public:
		GLFWDropCallBackMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eGLFWDropCallBackMessage;
		}

		std::vector<std::string> m_paths;
	};

}