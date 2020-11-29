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

int main(int argc, char* argv[]) {

	glm::vec3 camera = { 0.0f, 0.0f, 0.0f };
	bndr::Window window(1280, 720, "My Bndr Window");
	bndr::Mesh mesh("assets/golden_cube.obj", bndr::BASIC_MODEL, { 1.0f, 0.0f, 0.0f, 1.0f });
	bndr::Clock clock;

	while (window.Update(clock.DeltaTime())) {

		window.Clear(0.5f, 0.75f, 1.0f, 1.0f);
		window.Flip();
	}
	return 0;
}