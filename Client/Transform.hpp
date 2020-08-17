#pragma once

#include "ExternalLibraryHeaders.h"

namespace TNAP {

	class Transform
	{
	private:
		glm::vec3 m_translation = glm::vec3(0);
		glm::vec3 m_rotation = glm::vec3(0);
		glm::vec3 m_scale = glm::vec3(1);
		glm::mat4 m_matrix = glm::mat4(1);

		bool m_needsUpdating{ false };

	public:
		Transform();
		~Transform();

		Transform calculateTransform(const Transform& argTransform) const;
		const glm::vec3 getForwardAxis() const;

		inline void setTranslation(const glm::vec3& argTranslation) { m_needsUpdating = true; m_translation = argTranslation; }
		inline const glm::vec3& getTranslation() const { return m_translation; }
		// Non const return of translation will update the transforms matrix
		// as translation values could be edited
		inline glm::vec3& getTranslation() { m_needsUpdating = true; return m_translation; }

		inline void setRotation(const glm::vec3& argRotation) { m_needsUpdating = true; m_rotation = argRotation; }
		inline const glm::vec3& getRotation() const { return m_rotation; }
		// Non const return of rotation will update the transforms matrix
		// as rotation values could be edited
		inline glm::vec3& getRotation() { m_needsUpdating = true; return m_rotation; }

		inline void setScale(const glm::vec3& argScale) { m_needsUpdating = true; m_scale = argScale; }
		inline const glm::vec3& getScale() const { return m_scale; }
		// Non const return of scale will update the transforms matrix
		// as scale values could be edited
		inline glm::vec3& getScale() { m_needsUpdating = true; return m_scale; }
		
		// Translation relative to the objects current rotation
		void translateLocal(const glm::vec3& argTranslation);
		// Translation relative to the world axes
		void translateWorld(const glm::vec3& argTranslation);
		void rotate(const glm::vec3& argRotation);
		void scale(const glm::vec3& argScale);

		void calculateMatrix();
		inline const glm::mat4& getMatrix() 
		{
			if (m_needsUpdating)
				calculateMatrix();

			return m_matrix; 
		}

		inline const bool needsUpdating() const { return m_needsUpdating; }
	};

}
