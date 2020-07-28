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

	const glm::mat4 Transform::getMatrix() const
	{

		
		/*
		glm::mat4 modelTransform(1);

		modelTransform = glm::translate(modelTransform, m_translation);

		modelTransform = glm::rotate(modelTransform, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
		modelTransform = glm::rotate(modelTransform, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
		modelTransform = glm::rotate(modelTransform, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));

		modelTransform = glm::scale(modelTransform, m_scale);
		*/
		
		glm::mat4 modelTransform(1);

		glm::mat4 translationMatrix(1);
		glm::mat4 rotationMatrix(1);
		glm::mat4 scaleMatrix(1);

		translationMatrix = glm::translate(translationMatrix, m_translation);

		glm::quat q(glm::radians(m_rotation));

		//rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
		//rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
		//rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));

		rotationMatrix = glm::toMat4(q);

		scaleMatrix = glm::scale(scaleMatrix, m_scale);

		// Calculate the models transform
		modelTransform = translationMatrix * rotationMatrix * scaleMatrix;
		

		return modelTransform;
	}

}
