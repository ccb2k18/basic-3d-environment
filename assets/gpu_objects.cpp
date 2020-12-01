#include "gpu_objects.h"

void GLClearErrors() {

	while (glGetError() != GL_NO_ERROR) {

		// get the errors until there are none
	}
}

std::ostream& operator<<(std::ostream& out, const glm::mat4x4& mat) {

	out << "\n" << mat[0][0] << "\t" << mat[0][1] << "\t" << mat[0][2] << "\t" << mat[0][3] << "\n";
	out << mat[1][0] << "\t" << mat[1][1] << "\t" << mat[1][2] << "\t" << mat[1][3] << "\n";
	out << mat[2][0] << "\t" << mat[2][1] << "\t" << mat[2][2] << "\t" << mat[2][3] << "\n";
	out << mat[3][0] << "\t" << mat[3][1] << "\t" << mat[3][2] << "\t" << mat[3][3] << "\n";
	return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<float>& vec) {

	for (const float& fl : vec) {

		out << fl << " ";
	}
	return out;
}

namespace bndr {

	// define the static screenSize
	float Window::screenSize[2];
	// define the static map of textures
	std::unordered_map<const char*, uint> Texture::loadedTextures;
	// define the static map of programs
	std::unordered_map<const char*, uint> Program::loadedPrograms;

	// string manipulation methods
	std::vector<std::string> SplitStr(const std::string& str, char delimiter) {

		std::vector<std::string> strVector;
		std::string myStr = "";
		for (const char& ch : str) {

			if (ch != delimiter) {

				myStr += ch;
			}
			else {

				strVector.push_back(myStr);
				myStr = "";
			}
		}
		// one final push back
		strVector.push_back(myStr);
		return strVector;
	}

	void ReplaceStrChar(std::string& str, char charToReplace, char replacement) {

		for (char& ch : str) {

			// replace the chars that need to be replaced
			if (ch == charToReplace) {

				ch = replacement;
			}
		}
	}

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
		//glDeleteTextures(1, &textureID);

	}

	// bndr::VertexArray method definitions

	VertexArray::VertexArray(std::vector<float> vertices, std::vector<uint> indices, bool usingNormals, bool usingTextures) {

		int stride = 7 * sizeof(float);
		// add to stride based on the extra parameters added
		if (usingNormals) {

			stride += 3 * sizeof(float);
		}
		if (usingTextures) {
		
			stride += 3 * sizeof(float);
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

		// change the attribIndex and byte offset accordingly
		// every vao will have position and color, but not every vao will have normals and texture coords
		int attribIndex = 1;
		int offset = 7;
		if (usingNormals) {

			attribIndex++;
			glEnableVertexAttribArray(attribIndex);
			glVertexAttribPointer(attribIndex, 3, GL_FLOAT, GL_FALSE, stride, (const void*)(offset * sizeof(float)));
			// add to the offset in case there are textures too
			offset += 3;

		}

		// make sure a texture is bound when you do the draw call otherwise you will get errors
		// make sure the appropriate program is also used
		if (usingTextures) {

			attribIndex++;
			// texture uv coords attrib pointer
			glEnableVertexAttribArray(attribIndex);
			glVertexAttribPointer(attribIndex, 2, GL_FLOAT, GL_FALSE, stride, (const void*)(offset * sizeof(float)));

			attribIndex++;
			offset += 2;
			// texture index attrib pointer
			glEnableVertexAttribArray(attribIndex);
			glVertexAttribPointer(attribIndex, 1, GL_FLOAT, GL_FALSE, stride, (const void*)(offset * sizeof(float)));
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

		// define the key to check
		std::string keyStr = "";
		for (const std::pair<const char*, uint>& p : shaderFileNameEnumPairs) {

			keyStr += p.first;
		}
		const char* key = keyStr.c_str();
		// check if the program exists
		if (Program::loadedPrograms.find(key) != Program::loadedPrograms.end()) {

			std::cout << "Program already exists!\n";
			// reuse existing program
			program = Program::loadedPrograms[key];
			// exit since compilation has already taken place
			return;
		}
		program = glCreateProgram();
		// store the program in loadedPrograms
		Program::loadedPrograms.insert({ key, program });
		std::vector<uint> ids(shaderFileNameEnumPairs.size());
		int i = 0;
		// for each shader compile it
		for (const std::pair<const char*, uint>& pair : shaderFileNameEnumPairs) {

			uint id = CompileShader(pair.first, pair.second);
			ids[i] = id;
			i++;
		}
		// link all the shaders together
		LinkProgram(ids);
		/*for (std::unordered_map<const char*, uint>::iterator p = Program::loadedPrograms.begin(); p != Program::loadedPrograms.end(); p++) {

			std::pair<const char*, uint> kv = *p;
			std::cout << "image File: " << kv.first << "\tid: " << kv.second << "\n";
		}
		std::cout << "\n\n\n";*/

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

			glUniformMatrix4fv(location, 1, GL_TRUE, dataBegin);
			break;
		default:
			break;
		}
		Unuse();
	}

	void Program::SetUniformValue(const char* uniformName, int* dataBegin, uint size) {

		Use();
		int location = glGetUniformLocation(program, uniformName);
		glUniform1iv(location, size, dataBegin);
		Unuse();
	}

	// bndr::Mesh method definitions

	Mesh::Mesh(const char* objFile, uint shaderProgramType, std::vector<float> color) {

		std::pair<std::vector<float>, std::vector<uint>> pair = Mesh::LoadObjFile(objFile, color);
		// create the vertex array object (and we are using normals)
		vao = new VertexArray(pair.first, pair.second, false, false);
		// switch on shader program type
		switch (shaderProgramType) {

		case bndr::DEFAULT:

			program = Program::Default();
			break;
		case bndr::BASIC_MODEL:

			program = Program::BasicModel();
			break;
		}

		// set the matrices to default values
		Translate(0.0f, 0.0f, 5.0f);
		Rotate(0.0f, {});
		Scale(0.125f, 0.125f, 0.125f);

		glm::mat4x4 cameraView(1.0f);
		CameraView(cameraView);
		Project(1.57f, 720.0f / 1280.0f, 0.1f, 100.0f);

		// set the light direction
		float lightDir[3] = { 0.0f, 0.0f, 1.0f };
		program.SetUniformValue("lightDir", lightDir, bndr::VEC3);

	}

	Mesh::Mesh(std::vector<float> vertices, std::vector<uint> indices, bool usingNormals, bool usingTextures) {

		vao = new VertexArray(vertices, indices, usingNormals, usingTextures);
		program = Program::BasicModel();

		// set the matrices to default values
		Translate(0.0f, 0.0f, 0.0f);
		Rotate(0.0f, {});
		Scale(1.0f, 1.0f, 1.0f);

		glm::mat4x4 cameraView(1.0f);
		CameraView(cameraView);
		Project(1.57f, 720.0f / 1280.0f, 0.1f, 100.0f);

		// set the light direction
		float lightDir[3] = { 0.0f, 0.0f, 1.0f };
		program.SetUniformValue("lightDir", lightDir, bndr::VEC3);

	}

	void Mesh::Translate(float xTrans, float yTrans, float zTrans) {

		glm::mat4x4 translation(1.0f);
		translation[0][3] = xTrans;
		translation[1][3] = yTrans;
		translation[2][3] = zTrans;
		trans = translation;
		program.SetUniformValue("translation", glm::value_ptr(translation), bndr::MAT4);
	}

	void Mesh::Rotate(float angle, const std::vector<uint>& axes) {

		glm::mat4x4 rotation(1.0f);
		// for each axis specified add this to our rotation matrix
		for (const uint& axis : axes) {

			switch (axis) {

			case bndr::X_AXIS:

				rotation *= glm::rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case bndr::Y_AXIS:

				rotation *= glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case bndr::Z_AXIS:

				rotation *= glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
				break;
			}
		}
		rot = rotation;
		// set the uniform
		program.SetUniformValue("rotation", glm::value_ptr(rotation), bndr::MAT4);
	}

	void Mesh::Scale(float xScale, float yScale, float zScale) {

		glm::mat4x4 scale = glm::scale(glm::vec3(xScale, yScale, zScale));
		sca = scale;
		program.SetUniformValue("scale", glm::value_ptr(scale), bndr::MAT4);
	}

	void Mesh::CameraView(glm::mat4x4& cameraMat) {

		camView = cameraMat;
		program.SetUniformValue("cameraView", glm::value_ptr(cameraMat), bndr::MAT4);
	}

	void Mesh::Project(float fov, float aspectRatio, float zNear, float zFar) {

		glm::mat4x4 projection = glm::perspective(fov, aspectRatio, zNear, zFar);
		float proj[16] = {

			aspectRatio * (1.0f / tanf(fov / 2.0f)), 0.0f, 0.0f, 0.0f,
			0.0f, (1.0f / tanf(fov / 2.0f)), 0.0f, 0.0f,
			0.0f, 0.0f, zFar / (zFar - zNear), (-zFar * zNear) / (zFar - zNear),
			0.0f, 0.0f, 1.0f, 0.0f

		};
		persp = projection;
		program.SetUniformValue("perspective", proj, bndr::MAT4);
	}

	void Mesh::Update(float deltaTime, const glm::vec4& cameraPos) {

		if (counter == 0) {

			glm::mat4x4 mvp = persp * camView * trans * rot * sca;
			std::cout << mvp;
			counter++;
		}
		Translate(cameraPos.x, cameraPos.y, cameraPos.z);
	}

	void Mesh::Render() {

		program.Use();
		vao->Bind();
		vao->Render();
		vao->Unbind();
		program.Unuse();
		
	}

	Mesh::~Mesh() {

		delete vao;
	}
}
