#pragma once

#include "Message.hpp"
#include "Material.hpp"

class GenerateMaterialMessage : public TNAP::Message
{
public:
	GenerateMaterialMessage(const std::string& argMaterialName, size_t& argHandle, const TNAP::EMaterialType argMaterialType)
		: TNAP::Message(),
		m_materialName(argMaterialName), m_handle(argHandle), m_materialType(argMaterialType)
	{
		m_messageType = EMessageType::eGenerateMaterialMessage;
	}

	std::string m_materialName{ "" };
	size_t& m_handle;
	TNAP::EMaterialType m_materialType{ TNAP::EMaterialType::eUnlit };
};
