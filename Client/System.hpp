#pragma once

namespace TNAP {

	class System
	{
	private:
		bool m_enabled;
	public:
		virtual void init();
		virtual void update();
		virtual void sendMessage();
		virtual void imGuiRender();
		inline void setEnabled(const bool argValue);
		inline const bool getEnabled() const;
	};

}
