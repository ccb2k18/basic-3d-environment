#include "assets/gpu_objects.h"

int main(int argc, char* argv[]) {

	bndr::Window window(720, 720, "My Bndr Window");
	bndr::VertexArray vao({
		-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
		}, {0, 1, 2, 2, 3, 0}, 1);
	vao.Bind();
	bndr::Texture tex("assets/images/test_images/grass.bmp", { {GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_T, GL_REPEAT}, {GL_TEXTURE_MIN_FILTER, GL_NEAREST}, {GL_TEXTURE_MAG_FILTER, GL_NEAREST} });
	vao.Unbind();
	bndr::Program program = bndr::Program::Textured(1);
	bndr::Clock clock;
	while (window.Update(clock.DeltaTime())) {

		window.Clear(0.5f, 0.75f, 1.0f, 1.0f);
		program.Use();
		vao.Bind();
		tex.Bind();
		vao.Render();
		tex.Unbind();
		vao.Unbind();
		program.Unuse();
		window.Flip();
	}
	return 0;
}