#pragma once

#include "Light.hpp"

namespace TNAP {

	class PointLight : public TNAP::Light
	{
	private:
		float m_range{ 0 };

	public:
		PointLight();
		~PointLight();

		virtual void update(const Transform& argTransform) override;

		inline void setRange(const float argRange) { m_range = argRange; }
		inline const float getRange() const { return m_range; }

#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
