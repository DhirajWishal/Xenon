// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Logs.hpp"

#include <imgui.h>

#ifdef XENON_DEBUG
#include <iostream>

#endif // XENON_DEBUG

void Logs::begin(std::chrono::nanoseconds delta)
{
	if (m_bIsOpen)
	{
		if (ImGui::Begin("Logs", &m_bIsOpen))
		{
			for (const auto& [message, level] : m_Messages)
			{
				switch (level)
				{
				case SPDLOG_LEVEL_INFO:
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), message.c_str());
					break;

				case SPDLOG_LEVEL_WARN:
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), message.c_str());
					break;

				case SPDLOG_LEVEL_ERROR:
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), message.c_str());
					break;

				case SPDLOG_LEVEL_CRITICAL:
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), message.c_str());
					break;

				default:
					ImGui::Text(message.c_str());
					break;
				}
			}
		}

		ImGui::End();
	}
}

void Logs::end()
{
}

void Logs::sink_it_(const spdlog::details::log_msg& msg)
{
	spdlog::memory_buf_t formatted;
	spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
	m_Messages.emplace_back(fmt::to_string(formatted), msg.level);
}

void Logs::flush_()
{
	// Do nothing here for now.
}
