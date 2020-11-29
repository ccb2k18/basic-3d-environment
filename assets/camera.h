#pragma once
#include "gpu_objects.h"

namespace bndr {

	class Camera {

		static glm::vec3 cameraPos;
		static float pitch;
		static float yaw;
	public:

		static void Translate(const glm::vec3& translation) {

			Camera::cameraPos += translation;
		}

		static glm::mat4x4 LookAt(const glm::vec3& target) {

			return glm::lookAt(Camera::cameraPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
		}
	};
	float Camera::pitch = 0.0f;
	float Camera::yaw = 0.0f;
	glm::vec3 Camera::cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
} 
