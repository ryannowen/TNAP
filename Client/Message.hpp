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
			eGetMaterialMessage,
			eGenerateMaterialMessage,
			eLogMessage,
			eLoadModelMessage
		};

	protected:
		EMessageType m_messageType{ EMessageType::eUnknown };

	public:
		Message()
		{}

		const EMessageType getMessageType() const { return m_messageType; }

	};
}