#pragma once
#include <iostream>
#include <chrono>
#include <vector>
#include <glew.h>
#include <glfw3.h>

typedef unsigned int uint;

namespace bndr {

	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::steady_clock::time_point timePoint;
	typedef std::chrono::duration<float> duration;

	// clock object to keep track of deltaTime
	class Clock {

		timePoint start;
		timePoint end;
		duration deltaTime;
		
	public:

		Clock() : start(clock::now()), end(clock::now()), deltaTime(end - start) {}
		inline float DeltaTime() { end = clock::now(); deltaTime = (end - start); start = end; return deltaTime.count(); }

	};

	class Window {

		GLFWwindow* window = nullptr;
	public:

		Window(int width, int height, const char* title);
		bool Update(float deltaTime);
		inline void Clear(float red, float green, float blue, float alpha) { glClearColor(red, green, blue, alpha); glClear(GL_COLOR_BUFFER_BIT); }
		inline void Flip() { glfwSwapBuffers(window); }
		~Window();

	};

	class VertexArray {

		// vao id
		uint vao;
		// vbo ids (positions and indices respectively)
		uint vbo[2];
		// indices size
		uint indicesSize;

	public:

		VertexArray(std::vector<float> vertices, std::vector<uint> indices);
		inline void Bind() { glBindVertexArray(vao); }
		inline void Unbind() { glBindVertexArray(0); }
		void Render();
	};
}

