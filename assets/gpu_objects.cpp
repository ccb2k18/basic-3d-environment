#include "gpu_objects.h"

void GLClearErrors() {

	while (glGetError() != GL_NO_ERROR) {

		// get the errors until there are none
	}
}

namespace bndr {

	// define the static screenSize
	float Window::screenSize[2];
	// define the static map of textures
	std::unordered_map<const char*, uint> Texture::loadedTextures;

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

		// define the static screen size vars for the renderer class in the future
		screenSize[0] = (float)width;
		screenSize[1] = (float)height;

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

	// bndr::Texture method definitions

	Texture::Texture(const char* imgFile, const std::vector<std::pair<uint, uint>>& paramPairs) {

		// check if the texture exists
		if (Texture::loadedTextures.find(imgFile) != Texture::loadedTextures.end()) {

			// get the already loaded id and quit the constructor
			textureID = Texture::loadedTextures[imgFile];
			glBindTexture(GL_TEXTURE_2D, textureID);
			return;
		}

		// generate and bind the texture
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// save the texture
		Texture::loadedTextures.insert({ imgFile, textureID });

		bool defineBorder = false;
		// set all texture parameters in the parameters list
		for (const std::pair<uint, uint> pair : paramPairs) {

			glTexParameteri(GL_TEXTURE_2D, pair.first, pair.second);
			// check if we need to define a border color
			if (pair.second == GL_CLAMP_TO_BORDER) { defineBorder = true; }
		}
		if (defineBorder) {

			// default is white
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		std::pair<uchar*, std::pair<uint, uint>> imgByteData = Texture::GetImgArray(imgFile);
		uchar* data = imgByteData.first;
		uint width = imgByteData.second.first;
		uint height = imgByteData.second.second;

		// define the texture image and generate the mipmap
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// deallocate memory
		delete[] data;

	}

	Texture::~Texture() {

		// take care of opengl texture
		glDeleteTextures(1, &textureID);

	}

	// bndr::VertexArray method definitions

	VertexArray::VertexArray(std::vector<float> vertices, std::vector<uint> indices, uint numTextures) {

		int stride = 7 * sizeof(float);
		if (numTextures > 0) {

			stride = 10 * sizeof(float);
		}
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, NULL);

		// color attrib pointer
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (const void*)(3 * sizeof(float)));

		// make sure a texture is bound otherwise you will get errors
		// make sure the appropriate program is also used
		if (numTextures > 0) {

			// texture uv coords attrib pointer
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(7 * sizeof(float)));

			// texture index attrib pointer
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (const void*)(9 * sizeof(float)));
		}

		indicesSize = indices.size();
		// create and define element array buffer
		glGenBuffers(1, &vbo[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(uint), &indices[0], GL_DYNAMIC_DRAW);

		// unbind the vertex array
		Unbind();
	}

	void VertexArray::UpdateBufferData(std::vector<float> vertices, std::vector<uint> indices) {

		Bind();
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(float), &vertices[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		indicesSize = indices.size();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indicesSize*sizeof(uint), &indices[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		Unbind();

	}

	void VertexArray::Render() {

		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, NULL);
	}

	VertexArray::~VertexArray() {

	}

	// bndr::Program method definitions

	Program::Program(const std::vector<std::pair<const char*, uint>>& shaderFileNameEnumPairs) {

		program = glCreateProgram();

		std::vector<uint> ids(shaderFileNameEnumPairs.size());
		int i = 0;
		for (const std::pair<const char*, uint>& pair : shaderFileNameEnumPairs) {

			uint id = CompileShader(pair.first, pair.second);
			ids[i] = id;
			i++;
		}
		LinkProgram(ids);

	}

	uint Program::CompileShader(const char* shaderFileName, uint shaderEnum) {

		// create the opengl shader and get the id
		uint shaderID = glCreateShader(shaderEnum);

		// load from the file
		std::ifstream shaderFile(shaderFileName, std::ios::in);

		// check if the file is open
		if (!shaderFile.is_open()) {

			std::cout << "Shader file failed to open\n";
			exit(-1);
		}

		// if it is open let's extract the data
		std::string shaderSource;
		std::stringstream shaderSrcBuff;
		shaderSrcBuff << shaderFile.rdbuf();
		shaderSource = shaderSrcBuff.str();
		const char* shaderSrc = shaderSource.c_str();

		// close the file
		shaderFile.close();

		// give the source code to opengl
		glShaderSource(shaderID, 1, &shaderSrc, NULL);
		// compile the shader
		glCompileShader(shaderID);

		int infoLogLen;
		// check for compiling errors
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0) {

			std::cout << "Shader compilation failed\n";
			exit(-1);
		}

		return shaderID;

	}

	void Program::LinkProgram(const std::vector<uint>& shaderIDs) {

		// attach all the shaders
		for (const uint& shader : shaderIDs) {

			glAttachShader(program, shader);
		}
		// link the program
		glLinkProgram(program);

		int infoLogLen;
		// check for linking errors
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
		if (infoLogLen > 0) {

			std::cout << "Program linking failed\n";
			exit(-1);
		}

		// detach and delete the shaders
		for (const uint& shader : shaderIDs) {

			glDetachShader(program, shader);
			glDeleteShader(shader);
		}
	}

	void Program::SetUniformValue(const char* uniformName, float* dataBegin, uint dataTypeEnum) {

		Use();
		int location = glGetUniformLocation(program, uniformName);
		switch (dataTypeEnum)
		{
		case 1:

			glUniform1f(location, dataBegin[0]);
		case 2:

			glUniform2f(location, dataBegin[0], dataBegin[1]);
			break;
		case 3:

			glUniform3f(location, dataBegin[0], dataBegin[1], dataBegin[2]);
			break;
		case 4:

			glUniform4f(location, dataBegin[0], dataBegin[1], dataBegin[2], dataBegin[3]);
			break;
		case 16:

			glUniformMatrix4fv(location, 1, GL_FALSE, dataBegin);
			break;
		default:
			break;
		}
		Unuse();
	}

	// bndr::Mesh method definitions

	Mesh::Mesh(const char* objFile, std::vector<const char*> TexBMPFiles) {


	}

	Mesh::~Mesh() {

		delete vao;
		delete program;
	}
}
