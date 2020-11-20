#include "gpu_objects.h"

void GLClearErrors() {

	while (glGetError() != GL_NO_ERROR) {

		// get the errors until there are none
	}
}


namespace bndr {

	// bndr::Window method definitions

	Window::Window(int width, int height, const char* title) {

		// initialize glfw
		if (!glfwInit()) {

			std::cout << "GLFW init failed\n";
			exit(-1);
		}

		// create window object
		window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (window == nullptr) {

			std::cout << "window creation failed\n";
			glfwTerminate();
			exit(-1);
		}

		// opengl context
		glfwMakeContextCurrent(window);

		// initialize glew
		if (glewInit() != GLEW_OK) {

			std::cout << "OpenGL context failed to create\n";
			glfwDestroyWindow(window);
			glfwTerminate();
			exit(-1);
		}

	}

	bool Window::Update(float deltaTime) {

		// check for events
		glfwPollEvents();

		if (glfwWindowShouldClose(window)) {

			return false;
		}
		return true;
	}

	// destroy GLFWwindow instance and terminate glfw
	Window::~Window() {

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	// bndr::VertexArray method definitions

	VertexArray::VertexArray(std::vector<float> vertices, std::vector<uint> indices) {

		// create the vao
		glGenVertexArrays(1, &vao);
		// bind it
		Bind();

		// create and define array buffer data
		glGenBuffers(1, &vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

		// position attrib pointer
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);

		indicesSize = indices.size();
		// create and define element array buffer
		glGenBuffers(1, &vbo[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(uint), &indices[0], GL_DYNAMIC_DRAW);

		// unbind the vertex array
		Unbind();
	}

	void VertexArray::Render() {

		Bind();
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, NULL);
		Unbind();
	}
}
