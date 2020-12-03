#include "assets/gpu_objects.h"

bool Update(float deltaTime, bndr::Window& win, bndr::Camera& camera, std::vector<bndr::Mesh*>& meshes) {

	// update the camera
	camera.Update(deltaTime);

	// update the meshes
	for (bndr::Mesh* mesh : meshes) {

		mesh->CameraView(camera.GetLookAtMatrix());
		mesh->Update(deltaTime);
	}

	return win.Update(deltaTime, camera);
}

int main(int argc, char* argv[]) {

	bndr::Window window(1280, 720, "My Bndr Window");
	bndr::Camera camera(25.0f, 6.0f, -13.0f);
	window.SetKeyCallBack(bndr::Camera::HandleKeyEvents);
	bndr::Mesh mesh("models/wacky_world.obj", bndr::BASIC_MODEL, { 1.0f, 0.0f, 1.0f, 1.0f });
	/*bndr::Mesh mesh2({
		-0.5f, -0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		}, {
			0, 1, 2, // front
			2, 3, 0,

			1, 4, 5, // top
			5, 2, 1,

			7, 6, 0, // bottom
			0, 3, 7,

			7, 3, 2, // right
			2, 5, 7,

			0, 6, 4, // left
			4, 1, 0,

			6, 7, 5, // back
			5, 4, 6
		}, false, false);*/
	std::vector<bndr::Mesh*> meshes = { &mesh };
	bndr::Clock clock;

	while (Update(clock.DeltaTime(), window, camera, meshes)) {

		glm::vec3 pos = camera.GetPos();
		//std::cout << pos.x << "\t" << pos.y << "\t" << pos.z << "\n";
		window.Clear(0.5f, 0.75f, 1.0f, 1.0f);
		mesh.Render();
		//mesh2.Render();
		window.Flip();
	}
	return 0;
}