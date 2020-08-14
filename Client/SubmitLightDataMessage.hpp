#pragma once

#include "Message.hpp"

#include <memory>
#include "Light.hpp"

namespace TNAP
{
	class SubmitLightDataMessage : public TNAP::Message
	{
	public:
		SubmitLightDataMessage()
			: TNAP::Message()
		{
			m_messageType = EMessageType::eSubmitLightDataMessage;
		}

		std::unique_ptr<SLightData> m_data;

	};

}