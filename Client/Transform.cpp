#include "Transform.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace TNAP {

	Transform::Transform()
	{
	}

	Transform::~Transform()
	{
	}

	Transform Transform::calculateTransform(const Transform& argTransform) const
	{
		Transform t;
		t.m_translation = m_translation + argTransform.m_translation;
		t.m_rotation = m_rotation + argTransform.m_rotation;
		t.m_scale = m_scale * argTransform.m_scale;
		return t;
	}

	const glm::vec3 Transform::getForwardAxis() const
	{
		glm::quat q(glm::radians(m_rotation));
		glm::mat4 rotationMatrix = glm::toMat4(q);

		glm::vec4 forward{ glm::inverse(rotationMatrix) * glm::vec4(0, 0, -1, 1) };

		return glm::vec3(forward);
	}

	void Transform::translateLocal(const glm::vec3& argTranslation)
	{
		m_matrix = glm::translate(m_matrix, argTranslation);
	}

	void Transform::translateWorld(const glm::vec3& argTranslation)
	{
		m_needsUpdating = true;
		m_translation += argTranslation;
	}

	void Transform::rotate(const glm::vec3& argRotation)
	{
		m_matrix = glm::rotate(m_matrix, glm::radians(argRotation.x), glm::vec3(1, 0, 0));
		m_matrix = glm::rotate(m_matrix, glm::radians(argRotation.y), glm::vec3(0, 1, 0));
		m_matrix = glm::rotate(m_matrix, glm::radians(argRotation.z), glm::vec3(0, 0, 1));
	}

	void Transform::scale(const glm::vec3& argScale)
	{
		m_matrix = glm::scale(m_matrix, argScale);
	}

	void Transform::calculateMatrix()
	{
		glm::mat4 translationMatrix(1);
		glm::mat4 rotationMatrix(1);
		glm::mat4 scaleMatrix(1);

		translationMatrix = glm::translate(translationMatrix, m_translation);

		glm::quat q(glm::radians(m_rotation));
		rotationMatrix = glm::toMat4(q);

		scaleMatrix = glm::scale(scaleMatrix, m_scale);

		m_matrix = translationMatrix * rotationMatrix * scaleMatrix;

		m_needsUpdating = false;
	}

}
