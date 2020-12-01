#include "assets/gpu_objects.h"

/*bndr::VertexArray vao({
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.75f, -0.75f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.75f, 0.25f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.25f, 0.25f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.25f, -0.75f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
		}, {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4}, true);
	bndr::Program program = bndr::Program::Textured(2);
	vao.Bind();
	bndr::Texture tex("assets/images/test_images/grass.bmp", { {GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_T, GL_REPEAT}, {GL_TEXTURE_MIN_FILTER, GL_NEAREST}, {GL_TEXTURE_MAG_FILTER, GL_NEAREST} });
	bndr::Texture tex2("assets/images/test_images/sand.bmp", { {GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_T, GL_REPEAT}, {GL_TEXTURE_MIN_FILTER, GL_NEAREST}, {GL_TEXTURE_MAG_FILTER, GL_NEAREST} });
	tex.BindUnit(0);
	tex2.BindUnit(1);
	int loc = program.GetUniformLocation("myTex");
	int arr[2] = { 0, 1 };
	program.SetUniformValue("myTex", arr, 2);
	vao.Unbind();*/

// global camera
static glm::vec4 gCamera(0.0f, 0.0f, 10.0f, 0.0f);

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

void keyCallBack(GLFWwindow* win, int key, int scancode, int action, int mods) {

	// forward Z press
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {

		moveZ = true;
		zSign = -1.0f;
	}
	// backward Z press
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {

		moveZ = true;
		zSign = 1.0f;
	}
	// Z release
	if ((key == GLFW_KEY_W || key == GLFW_KEY_S) && action == GLFW_RELEASE) {

		moveZ = false;
		zSign = 1.0f;
	}

	// forward X press
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {

		moveX = true;
		xSign = -1.0f;
	}
	// backward X press
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {

		moveX = true;
		xSign = 1.0f;
	}
	// X release
	if ((key == GLFW_KEY_D || key == GLFW_KEY_A) && action == GLFW_RELEASE) {

		moveX = false;
		xSign = 1.0f;
	}

	// forward Y press
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {

		moveY = true;
		ySign = -1.0f;
	}
	// backward Y press
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {

		moveY = true;
		ySign = 1.0f;
	}
	// Y release
	if ((key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) && action == GLFW_RELEASE) {

		moveY = false;
		ySign = 1.0f;
	}

}


bool Update(float deltaTime, bndr::Window& win, std::vector<bndr::Mesh*>& meshes) {

	if (moveX) {

		gCamera.x += deltaTime * xSign * speed;
	}
	if (moveY) {

		gCamera.y += deltaTime * ySign * speed;
	}
	if (moveZ) {

		gCamera.z += deltaTime * zSign * speed;
	}

	// update the meshes
	for (bndr::Mesh* mesh : meshes) {

		mesh->Update(deltaTime, gCamera);
	}

	return win.Update(deltaTime);
}

int main(int argc, char* argv[]) {

	bndr::Window window(1280, 720, "My Bndr Window");
	window.SetKeyCallBack(keyCallBack);
	bndr::Mesh mesh("models/s_tetris.obj", bndr::BASIC_MODEL, { 0.5f, 0.0f, 0.0f, 1.0f });
	bndr::Mesh mesh2({
		-0.5f, -0.5f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
		}, {
			0, 1, 2,
			2, 3, 0,
			1, 4, 5,
			5, 2, 1
		}, false, false);
	std::vector<bndr::Mesh*> meshes = { &mesh, &mesh2 };
	bndr::Clock clock;

	while (Update(gDeltaTime, window, meshes)) {

		//std::cout << gCamera.x << "\t" << gCamera.y << "\t" << gCamera.z << "\n";
		gDeltaTime = clock.DeltaTime();
		window.Clear(0.25f, 0.375f, 0.5f, 1.0f);
		mesh.Render();
		//mesh2.Render();
		window.Flip();
	}
	return 0;
}