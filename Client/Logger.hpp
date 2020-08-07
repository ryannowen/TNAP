#pragma once

#include <vector>

#include "System.hpp"
#include "ImGuiInclude.hpp"

namespace TNAP
{

	class Logger : public System
	{
	private:
		inline void ClearLog();

#if USE_IMGUI
		ImGuiTextBuffer m_buffer;
		ImGuiTextFilter m_filter;
		ImVector<int> m_lineOffsets;
		std::vector<ImVec4> m_lineColours;

		bool m_autoScroll{ true };
		bool m_scrollToBottom{ true };

		void AddLog(const char* argMessage, ...);
#endif
	public:
		Logger();
		~Logger();

		virtual void init() override;
		virtual void sendMessage(TNAP::Message* const argMessage) override;

#if USE_IMGUI	
		virtual void imGuiRender() override;
#endif
	};

}
