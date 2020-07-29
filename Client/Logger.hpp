#pragma once

#include "System.hpp"
#include "ImGuiInclude.hpp"

namespace TNAP
{

	class Logger : public System
	{
	private:
		ImGuiTextBuffer m_buffer;
		ImGuiTextFilter m_filter;
		ImVector<int> m_lineOffsets;
		bool m_autoScroll{ false };
		bool m_scrollToBottom{ false };

	public:
		Logger();
		~Logger();

		virtual void init() override;
		virtual void sendMessage(TNAP::Message* const argMessage) override;
		inline void ClearLog();
		void AddLog(const char* argMessage, ...);
#if USE_IMGUI
		virtual void imGuiRender() override;
#endif
	};

}
