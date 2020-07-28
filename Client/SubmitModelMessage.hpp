#pragma once
#include "Message.hpp"
#include "ExternalLibraryHeaders.h"
#include "Transform.hpp"
#include <vector>

namespace TNAP
{

	class SubmitModelMessage : public TNAP::Message
	{
	public:
		SubmitModelMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eSubmitModelMessage;
		}

		size_t m_modelHandle{ 0 };
		std::vector<size_t>* m_materialHandle{ nullptr };
		glm::mat4 m_transform{ glm::mat4(1) };
	};

}