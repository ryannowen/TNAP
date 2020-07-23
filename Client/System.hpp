#pragma once

namespace TNAP {

	class System
	{
	private:
		bool m_enabled{ true };
	public:
		System();

		virtual void init();
		virtual void update();
		virtual void sendMessage();
		virtual void imGuiRender();
		inline void setEnabled(const bool argValue) { m_enabled = argValue; }
		inline const bool getEnabled() const { return m_enabled; }
	};

}
