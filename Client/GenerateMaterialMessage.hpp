#pragma once

#include "Message.hpp"
#include "Material.hpp"

class GenerateMaterialMessage : public TNAP::Message
{
public:
	GenerateMaterialMessage(size_t& argHandle, const TNAP::EMaterialType argMaterialType)
		: TNAP::Message(),
		m_handle(argHandle), m_materialType(argMaterialType)
	{
		m_messageType = EMessageType::eGenerateMaterialMessage;
	}

	size_t& m_handle;
	TNAP::EMaterialType m_materialType{ TNAP::EMaterialType::eUnlit };
};
