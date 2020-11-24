#include "assets/gpu_objects.h"

int main(int argc, char* argv[]) {

	bndr::Window window(1280, 720, "My Bndr Window");
	bndr::Program program = bndr::Program::TexturedNoColor();
	//float color[4] = { 1.0f, 0.5f, 0.0f, 1.0f };
	//program.SetUniformValue("userColor", color, bndr::VEC4);
	bndr::VertexArray va({
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   0.5f, 0.5f, 0.5f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.5f, 0.5f, 0.5f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.5f, 0.5f, 0.5f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   0.5f, 0.5f, 0.5f,   0.0f, 1.0f    // top left 
		}, { 0, 1, 2, 2, 3, 0 }, "assets/images/gold.bmp", { {GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_S, GL_REPEAT}, {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
		{GL_TEXTURE_MAG_FILTER, GL_LINEAR} });
	bndr::VertexArray va2({

		-0.75f, -0.4f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.75f, 0.4f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.4f, 0.4f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.4f, -0.4f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
		}, {0, 1, 2, 3, 0, 2}, "assets/images/dirt.bmp", { {GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_S, GL_REPEAT}, {GL_TEXTURE_MIN_FILTER, GL_LINEAR},
		{GL_TEXTURE_MAG_FILTER, GL_LINEAR} });
	bndr::Clock clock;
	while (window.Update(clock.DeltaTime())) {

		window.Clear(0.5f, 0.75f, 1.0f, 1.0f);
		program.Use();
		va.Render();
		va2.Render();
		program.Unuse();
		window.Flip();
	}
	return 0;
}