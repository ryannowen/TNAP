#include "Logger.hpp"
#include "LogMessage.hpp"

#include <iostream>

namespace TNAP
{
	inline void Logger::ClearLog()
	{
		m_buffer.clear();
		m_lineOffsets.clear();
	}

	void Logger::AddLog(const char* argMessage, ...)
	{
		int old_size = m_buffer.size();
		va_list args;
		va_start(args, argMessage);
		m_buffer.appendfv(argMessage, args);
		va_end(args);

		for (int new_size = m_buffer.size(); old_size < new_size; old_size++)
			if (m_buffer[old_size] == '\n')
				m_lineOffsets.push_back(old_size);

		if (m_autoScroll)
			m_scrollToBottom = true;
	}

	Logger::Logger()
	{
	}
	Logger::~Logger()
	{
	}
	void Logger::init()
	{
	}

	void Logger::sendMessage(TNAP::Message* const argMessage)
	{
		static const std::vector<ImVec4> colours
		{
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
			ImVec4(0.85f, 0.41f, 0.09, 1.0f),
			ImVec4(1.0f, 0, 0, 1.0f),
			ImVec4(0, 1.0f, 0, 1.0f)
		};

		switch (argMessage->getMessageType())
		{
			case Message::EMessageType::eLogMessage:
			{
				LogMessage* const logMessage{ static_cast<LogMessage*>(argMessage) };
				if (logMessage->m_message.back() != *"\n")
					logMessage->m_message.append("\n");
				std::string message{ "" };

				switch (logMessage->m_logType)
				{
				case LogMessage::ELogType::eInfo:
					message = "[Info] " + logMessage->m_message;
					m_lineColours.push_back(colours.at(0));
					break;

				case LogMessage::ELogType::eWarning:
					message = "[Warning] " + logMessage->m_message;
					m_lineColours.push_back(colours.at(1));
					break;

				case LogMessage::ELogType::eError:
					message = "[Error] " + logMessage->m_message;
					m_lineColours.push_back(colours.at(2));
					break;

				case LogMessage::ELogType::eSuccess:
					message = "[Success] " + logMessage->m_message;
					m_lineColours.push_back(colours.at(3));
					break;

				default:
					message = "[ERROR] Unknown Log Type, : " + logMessage->m_message;
					m_lineColours.push_back(colours.at(2));
					break;
				}

#if USE_IMGUI
				AddLog(message.c_str());
#endif
			}
			break;

		default:
			break;
		}
	}

#if USE_IMGUI
	

	void TNAP::Logger::imGuiRender()
	{
		static bool showLogger{ true };
		ImGui::Begin("Logger", &showLogger);
		{

			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				ImGui::Checkbox("Auto-scroll", &m_autoScroll);
				ImGui::EndPopup();
			}
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("Options");

			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			m_filter.Draw("Filter", -100.0f);

			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (clear)
				ClearLog();

			const char* buf_begin = m_buffer.begin();
			const char* line = buf_begin;
			ImGui::SetWindowFontScale(0.8f);
			for (int line_no = 0; line != NULL; line_no++)
			{
				bool addText{ true };
				const char* line_end = (line_no < m_lineOffsets.Size) ? buf_begin + m_lineOffsets[line_no] : NULL;

				if (m_filter.IsActive())
				{
					if (!m_filter.PassFilter(line, line_end))
						addText = false;
				}

				if (addText)
				{
					ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, m_lineColours.at(line_no));
					ImGui::TextUnformatted(line, line_end);
					ImGui::PopStyleColor(1);
				}

				line = line_end && line_end[1] ? line_end + 1 : NULL;
			}

			// Scroll to bottom of logs
			if (m_scrollToBottom)
				ImGui::SetScrollHere(1.0f);
			m_scrollToBottom = false;
		}

		ImGui::SetWindowFontScale(1);

		ImGui::EndChild();
		ImGui::End();
	}
#endif

}
