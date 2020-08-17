#pragma once

#include "Message.hpp"

#include <vector>
#include <string>

namespace TNAP
{

	class Material;

	class GetModelInfoMessage : public TNAP::Message
	{
	public:
		GetModelInfoMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eGetModelInfoMessage;
		}

		size_t m_modelHandle{ 0 };

		std::string m_filepath{ "" };
		std::vector<size_t> m_defaultMaterialHandles;
	};

}