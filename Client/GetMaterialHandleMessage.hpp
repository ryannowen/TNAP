#pragma once

#include "Message.hpp"
#include <string>

namespace TNAP
{

	class Material;

	class GetMaterialHandleMessage : public TNAP::Message
	{
	public:
		GetMaterialHandleMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eGetMaterialHandleMessage;
		}

		std::string m_materialName{ "" };
		size_t m_materialHandle{ 0 };

	};

}