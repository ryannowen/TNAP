#include "System.hpp"

namespace TNAP {

	void System::init()
	{
	}

	void System::update()
	{
	}

	void System::sendMessage()
	{
	}

	void System::imGuiRender()
	{
	}

	void System::setEnabled(const bool argValue)
	{
		m_enabled = argValue;
	}

	inline const bool System::getEnabled() const
	{
		return m_enabled;
	}

}