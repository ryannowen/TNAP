#pragma once

#include "Message.hpp"
#include <string>
#include <vector>

namespace TNAP
{

	class Material;

	class LoadModelMessage : public TNAP::Message
	{
	public:
		LoadModelMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eLoadModelMessage;
		}

		std::string m_modelName{ "" };
		std::string m_modelFilepath{ "" };
		size_t m_modelHandle{ 0 };
		std::vector<size_t> m_materialHandles{ 0 };
	};

}