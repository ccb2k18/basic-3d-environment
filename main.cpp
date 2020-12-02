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
	bndr::Camera camera(0.0f, 0.0f, 10.0f);
	window.SetKeyCallBack(bndr::Camera::HandleKeyEvents);
	bndr::Mesh mesh("models/s_tetris.obj", bndr::BASIC_MODEL, { 0.5f, 0.0f, 0.0f, 1.0f });
	bndr::Mesh mesh2({
		-0.5f, -0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
		}, {
			0, 1, 2,
			2, 3, 0,
			1, 4, 5,
			5, 2, 1
		}, false, false);
	std::vector<bndr::Mesh*> meshes = { &mesh, &mesh2 };
	bndr::Clock clock;

	while (Update(clock.DeltaTime(), window, camera, meshes)) {

		//std::cout << bndr::cmra::position.x << "\t" << bndr::cmra::position.y << "\t" << bndr::cmra::position.z << "\n";
		window.Clear(0.25f, 0.375f, 0.5f, 1.0f);
		mesh.Render();
		mesh2.Render();
		window.Flip();
	}
	return 0;
}