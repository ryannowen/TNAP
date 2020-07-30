#pragma once

#include "System.hpp"
#include "ImGuiInclude.hpp"

namespace TNAP
{

	class Logger : public System
	{
	private:
#if USE_IMGUI
		ImGuiTextBuffer m_buffer;
		ImGuiTextFilter m_filter;
		ImVector<int> m_lineOffsets;
#endif
		bool m_autoScroll{ false };
		bool m_scrollToBottom{ false };

	public:
		Logger();
		~Logger();

		virtual void init() override;
		virtual void sendMessage(TNAP::Message* const argMessage) override;
#if USE_IMGUI
		inline void ClearLog();
		void AddLog(const char* argMessage, ...);
		virtual void imGuiRender() override;
#endif
	};

}
