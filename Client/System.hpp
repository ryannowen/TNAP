#pragma once

#include "Message.hpp"

namespace TNAP {

	class System
	{
	private:
		bool m_enabled{ true };
	public:
		System();

		virtual void init();
		virtual void update();
		virtual void sendMessage(TNAP::Message* const argMessage);
		virtual void imGuiRender();
		inline void setEnabled(const bool argValue) { m_enabled = argValue; }
		inline const bool getEnabled() const { return m_enabled; }
	};

}
