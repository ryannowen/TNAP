#pragma once

#include "PointLight.hpp"

namespace TNAP {

	class SpotLight : public PointLight
	{
	private:
		float m_fov{ 0 };

	public:
		SpotLight();
		~SpotLight();

		virtual void update(const glm::mat4& parentTransform) override;

		inline void setFov(const float argFov) { m_fov = argFov; }
		inline const float getFov() const { return m_fov; }

#if USE_IMGUI
		virtual void imGuiRenderProperties() override;
#endif
	};

}
