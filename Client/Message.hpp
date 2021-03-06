#pragma once

namespace TNAP
{

	class Message
	{
	public:
		enum class EMessageType
		{
			eUnknown,
			eGetTextureMessage,
			eLoadTextureMessage,
			eSubmitModelMessage,
			eSubmitLightDataMessage,
			eGetMaterialMessage,
			eGenerateMaterialMessage,
			eLogMessage,
			eLoadModelMessage,
			eGLFWDropCallBackMessage,
			eGetModelInfoMessage,
			eGetMaterialHandleMessage
		};

	protected:
		EMessageType m_messageType{ EMessageType::eUnknown };

	public:
		Message()
		{}

		const EMessageType getMessageType() const { return m_messageType; }

	};
}