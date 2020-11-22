#include "assets/gpu_objects.h"

int main(int argc, char* argv[]) {

	bndr::Window window(1280, 720, "My Bndr Window");
	bndr::Program program = bndr::Program::Default();
	float color[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
	program.SetUniformValue("userColor", color, bndr::VEC4);
	bndr::VertexArray va({ 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f, 0.5f, 0.5f, 0.0f }, { 0, 1, 2, 0, 2, 3});
	bndr::Clock clock;
	while (window.Update(clock.DeltaTime())) {

		window.Clear(1.0f, 0.0f, 0.0f, 1.0f);
		program.Use();
		va.Render();
		program.Unuse();
		window.Flip();
	}
	return 0;
}