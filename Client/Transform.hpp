#pragma once

#include "ExternalLibraryHeaders.h"

namespace TNAP {

	class Transform
	{
	private:
		glm::vec3 m_translation = glm::vec3(0);
		glm::vec3 m_rotation = glm::vec3(0);
		glm::vec3 m_scale = glm::vec3(1);

	public:
		Transform();
		~Transform();

		Transform calculateTransform(const Transform& argTransform) const;
		const glm::vec3 getForwardAxis() const;

		inline void setTranslation(const glm::vec3& argTranslation) { m_translation = argTranslation; }
		inline const glm::vec3& getTranslation() const { return m_translation; }
		inline glm::vec3& getTranslation() { return m_translation; }

		inline void setRotation(const glm::vec3& argRotation) { m_rotation = argRotation; }
		inline const glm::vec3& getRotation() const { return m_rotation; }
		inline glm::vec3& getRotation() { return m_rotation; }

		inline void setScale(const glm::vec3& argScale) { m_scale = argScale; }
		inline const glm::vec3& getScale() const { return m_scale; }
		inline glm::vec3& getScale() { return m_scale; }

		const glm::mat4 getMatrix() const;
	};

}
