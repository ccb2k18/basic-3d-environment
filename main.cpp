#include "assets/gpu_objects.h"

// global camera
static glm::vec4 gCamera(0.0f, 0.0f, 10.0f, 0.0f);

static float yaw = 1.57f;
static float pitch = 0.0f;

// the global delta time
static float gDeltaTime = 0.0f;
// controls movement
static bool moveX = false;
static bool moveY = false;
static bool moveZ = false;
// sign for direction
static float xSign = 1.0f;
static float ySign = 1.0f;
static float zSign = 1.0f;

static float speed = 2.0f;

// mouse position
static int mousePos[2] = { 0, 0 };


bool Update(float deltaTime, bndr::Window& win, std::vector<bndr::Mesh*>& meshes) {

	// update the camera
	bndr::cmra::update(deltaTime);

	// update the meshes
	for (bndr::Mesh* mesh : meshes) {

		mesh->Update(deltaTime, bndr::cmra::position);
	}

	return win.Update(deltaTime);
}

int main(int argc, char* argv[]) {

	bndr::Window window(1280, 720, "My Bndr Window");
	window.SetKeyCallBack(bndr::cmra::keyCallBack);
	window.SetCursorPosCallback(bndr::cmra::cursorCallBack);
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

	while (Update(gDeltaTime, window, meshes)) {

		//std::cout << bndr::cmra::position.x << "\t" << bndr::cmra::position.y << "\t" << bndr::cmra::position.z << "\n";
		gDeltaTime = clock.DeltaTime();
		window.Clear(0.25f, 0.375f, 0.5f, 1.0f);
		mesh.Render();
		mesh2.Render();
		window.Flip();
	}
	return 0;
}