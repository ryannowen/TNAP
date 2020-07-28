#pragma once

#include "Message.hpp"
#include <vector>

namespace TNAP
{

	class Material;

	class GetMaterialMessage : public TNAP::Message
	{
	public:
		GetMaterialMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eGetMaterialMessage;
		}

		std::vector<size_t>* m_materialHandle{ nullptr };
		std::vector<Material*> m_materialVector;
	};

}