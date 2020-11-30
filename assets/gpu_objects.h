#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <glew.h>
#include <glfw3.h>
#include <cmath>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform.hpp>
//#include <glm.hpp>

typedef unsigned int uint;
typedef unsigned char uchar;

std::ostream& operator<<(std::ostream& out, const glm::mat4x4& mat);
std::ostream& operator<<(std::ostream& out, const std::vector<float>& vec);

namespace bndr {

	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::steady_clock::time_point timePoint;
	typedef std::chrono::duration<float> duration;
	
	std::vector<std::string> SplitStr(const std::string& str, char delimiter = ' ');
	void ReplaceStrChar(std::string& str, char charToReplace, char replacement);

	// clock object to keep track of deltaTime
	class Clock {

		timePoint start;
		timePoint end;
		duration deltaTime;
		
	public:

		Clock() : start(clock::now()), end(clock::now()), deltaTime(end - start) {}
		inline float DeltaTime() { end = clock::now(); deltaTime = (end - start); start = end; return deltaTime.count(); }

	};

	// window class to draw to with opengl
	class Window {

		GLFWwindow* window = nullptr;
		// screen dimension variables {width, height}
		static float screenSize[2];
	public:

		Window(int width, int height, const char* title);
		bool Update(float deltaTime);
		inline void SetKeyCallBack(void (*keyCallback)(GLFWwindow*, int, int, int, int)) { glfwSetKeyCallback(window, keyCallback); }
		inline void Clear(float red, float green, float blue, float alpha) { glClearColor(red, green, blue, alpha); glClear(GL_COLOR_BUFFER_BIT); }
		inline void Flip() { glfwSwapBuffers(window); }
		~Window();

	};

	// class for abstracting gl textures away
	// assumes a 2d texture file
	// if you set the vertex array constructor argument "numTextures" to greater than 0
	// the stride will be 40 as opposed to 28
	class Texture {

		uint textureID;
		// this is where we will store the texture ids in case the user tries to load a texture that already exists
		static std::unordered_map<const char*, uint> loadedTextures;
		// this is private because otherwise memory leaks will occur
		static std::pair<uchar*, std::pair<uint, uint>> GetImgArray(const char* imgFile) {

			FILE* imgBuffer = fopen(imgFile, "rb");
			if (!imgBuffer) { throw std::exception("Could not open file\n"); }
			uchar header[54];
			uint dataPos, imageSize, width, height;
			if (fread(header, 1, 54, imgBuffer) != 54) { // If not 54 bytes read : problem
				
				throw std::exception("Not a correct BMP file\n");
			}
			if (header[0] != 'B' || header[1] != 'M') {
				
				throw std::exception("Not a correct BMP file\n");
			}
			// Read ints from the byte array
			dataPos = *(int*)&(header[0x0A]);
			imageSize = *(int*)&(header[0x22]);
			width = *(int*)&(header[0x12]);
			height = *(int*)&(header[0x16]);
			// Some BMP files are misformatted, guess missing information
			if (imageSize == 0) { imageSize = width * height * 3; } // 3 : one byte for each Red, Green and Blue component
			if (dataPos == 0) { dataPos = 54; } // The BMP header is done that way
			uchar* imgBytes = new uchar[imageSize];
			// Read the actual data from the file into the buffer
			fread(imgBytes, 1, imageSize, imgBuffer);

			//Everything is in memory now, the file can be closed
			fclose(imgBuffer);
			return { imgBytes, {width, height} };
		}

	public:

		Texture(const char* bmpFile, const std::vector<std::pair<uint, uint>>& paramPairs);
		inline void Bind() { glBindTexture(GL_TEXTURE_2D, textureID); }
		inline void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
		// get the texture opengl id
		inline uint GetID() { return textureID; }
		inline void BindUnit(uint index) { glBindTextureUnit(index, textureID); }
		// create a texture with default settings
		static Texture Default(const char* bmpFile) {

			return Texture(bmpFile, { {GL_TEXTURE_WRAP_S, GL_REPEAT},
		{GL_TEXTURE_WRAP_T, GL_REPEAT}, {GL_TEXTURE_MIN_FILTER, GL_LINEAR}, {GL_TEXTURE_MAG_FILTER, GL_LINEAR} });
		}
		~Texture();
	};

	// object for containing a vertex array object as well as vertex buffer data
	class VertexArray {

		// vao id
		uint vao;
		// vbo ids (positions and indices respectively)
		uint vbo[2];
		// indices size
		uint indicesSize;

	public:

		// usingTextures and usingNormals is true: x y z r g b a nx ny nz u v i = stride of 13 * sizeof(float)
		// only usingNormals is true: x y z r g b a nx ny nz = stride of 10 * sizeof(float)
		// not using either: x y z r g b a = stride of 7 * sizeof(float)
		VertexArray(std::vector<float> vertices, std::vector<uint> indices, bool usingNormals = false, bool usingTextures = false);
		// update the buffer data with new vertices and indices
		void UpdateBufferData(std::vector<float> vertices, std::vector<uint> indices);
		// load a texture to use with the vao (make sure the vertices specify color coords and texture coords)
		inline void Bind() { glBindVertexArray(vao); }
		inline void Unbind() { glBindVertexArray(0); }
		void Render();
		~VertexArray();
	};

	enum rotationAxes {

		X_AXIS = 0,
		Y_AXIS = 1,
		Z_AXIS = 2
	};

	enum shaderTypes {

		VERT_SHDR = GL_VERTEX_SHADER,
		FRAG_SHDR = GL_FRAGMENT_SHADER,
		GEOM_SHDR = GL_GEOMETRY_SHADER
	};

	enum uniformDataTypes {

		FLOAT = 1,
		VEC2 = 2,
		VEC3 = 3,
		VEC4 = 4,
		MAT4 = 16
	};

	enum shaderProgramTypes {

		DEFAULT = 0,
		BASIC_MODEL = 1
	};

	// shader program class
	class Program {

		// program id
		uint program;
		// store the already loaded programs
		static std::unordered_map<const char*, uint> loadedPrograms;

	public:

		// pass in a vector of shaderFileNames with their corresponding enums (ie {{"myVertexShader.glsl", GL_VERTEX_SHADER},...})
		Program(const std::vector<std::pair<const char*, uint>>& shaderFileNameEnumPairs);
		// default constructor
		Program() : program((uint)0) {}
		// compile an individual shader
		uint CompileShader(const char* shaderFileName, uint shaderEnum);
		// link the entire program
		void LinkProgram(const std::vector<uint>& shaderIDs);
		inline void Use() { glUseProgram(program); }
		inline void Unuse() { glUseProgram(0); }
		void SetUniformValue(const char* uniformName, float* dataBegin, uint dataTypeEnum);
		// set integer values
		void SetUniformValue(const char* uniformName, int* dataBegin, uint size);
		inline int GetUniformLocation(const char* uniformName) { return glGetUniformLocation(program, uniformName); }
		// returns a basic shader that colors a shape one color
		static Program Default() {

			return Program({ {"shaders/default_shdr/default_vert.glsl", bndr::VERT_SHDR}, {"shaders/default_shdr/default_frag.glsl", bndr::FRAG_SHDR} });
		}
		// returns a shader program for textured objects (pass in number of textures to load (it shall not exceed 6))
		static Program Textured(uint numTextures) {

			if (numTextures == 0 || numTextures == 1) {

				return Program({ {"shaders/texture_shdr/texture_vert.glsl", bndr::VERT_SHDR}, {"shaders/texture_shdr/texture_frag.glsl", bndr::FRAG_SHDR} });
			}
			else if (numTextures > 1 && numTextures < 7) {

				std::string texFrag = "shaders/texture_shdr/textures" + std::to_string(numTextures) + "_frag.glsl";
				const char* texFragCStr = texFrag.c_str();
				return Program({ {"shaders/texture_shdr/texture_vert.glsl", bndr::VERT_SHDR}, {texFragCStr, bndr::FRAG_SHDR} });
			}
			else {

				throw std::exception("The maximum number of textures per fragment shader is 6\n");
			}
		}
		// create a program for a basic model (3d model no texture)
		static Program BasicModel() {

			return Program({ {"shaders/basic_model_shdr/basic_model_vert.glsl", bndr::VERT_SHDR}, {"shaders/basic_model_shdr/basic_model_frag.glsl", bndr::FRAG_SHDR} });
		}
	};

	// mesh that contains vertices, colors, normals, and can load it all from a blender object file
	class Mesh {

		VertexArray* vao;
		Program program;
		glm::mat4x4 trans;
		glm::mat4x4 rot;
		glm::mat4x4 sca;
		glm::mat4x4 camView;
		glm::mat4x4 persp;
		int counter = 0;

	public:

		// from object file
		Mesh(const char* objFile, uint shaderProgramType, std::vector<float> color = {1.0f, 1.0f, 1.0f, 1.0f});
		// user entered
		Mesh(std::vector<float> vertices, std::vector<uint> indices, bool usingNormals = false, bool usingTextures = false);
		// methods to update uniforms
		void Translate(float xTrans, float yTrans, float zTrans);
		void Rotate(float angle, const std::vector<uint>& axes);
		void Scale(float xScale, float yScale, float zScale);

		void CameraView(glm::mat4x4& cameraMat);
		void Project(float fov, float aspectRatio, float zNear, float zFar);
		// static method to load an object file
		static std::pair<std::vector<float>, std::vector<uint>> LoadObjFile(const char* objFile, std::vector<float> color){

			bool objectHasColors = false;
			std::vector<float> temp = { 1.0f, 1.0f, 1.0f, 1.0f };
			// if the default color has not been changed then it is assumed the object file has colors
			if (color == temp) { objectHasColors = true; }
			// create a ifstream
			std::ifstream objStream(objFile, std::ios::in);
			// check if the file is open
			if (!objStream.is_open()) {

				throw std::runtime_error("Cannot open object file\n");
			}

			// 2D just refers to the dimensions of the vector and has nothing to do with the data itself
			std::vector<std::vector<float>> vertices2D;
			std::vector<std::vector<float>> normals2D;
			std::vector<uint> indices;
			std::string line;
			// current position index to add positions to in vertices2D
			int posIndex = 0;
			// current normals index to add normals to in normals2D
			int normalsIndex = 0;
			while (std::getline(objStream, line)) {

				// these are the position vertices (they are always first)
				if (line[0] == 'v' && line[1] == ' ') {

					// add an empty vector to pack with data
					vertices2D.push_back({});
					// split up the data in each line
					std::vector<std::string> splitLine = SplitStr(line, ' ');
					// we skip the v so we set i to 1
					for (int i = 1; i < splitLine.size(); i++) {

						vertices2D[posIndex].push_back(std::stof(splitLine[i]));
					}
					// now add the default color if the object file doesn't specify it
					if (!objectHasColors) {

						for (float& fl : color) {

							vertices2D[posIndex].push_back(fl);
						}
					}
					// increment the current index
					posIndex++;
				}
				// we are loading normal data
				/*else if (line[0] == 'v' && line[1] == 'n') {

					// add the empty vector to put the normals in
					normals2D.push_back({});
					std::vector<std::string> splitLine = SplitStr(line, ' ');
					// skip 0 so we don't add vn
					for (int i = 1; i < splitLine.size(); i++) {

						normals2D[normalsIndex].push_back(std::stof(splitLine[i]));
					}
					// increment the normalsIndex
					normalsIndex++;
				}*/

				// faces (indices to vertices and normals)
				else if (line[0] == 'f' && line[1] == ' ') {

					std::vector<std::string> splitLine = SplitStr(line, ' ');
					// splitLine has a pair of indices, one for the positions and one for the normals
					// therefore we must use the normal index to add the appropriate normals to the vertices vector
					for (int i = 1; i < splitLine.size(); i++) {

						// split the element even further (i.e. "5/4" becomes {"5", "4"})
						std::vector<std::string> pair = SplitStr(splitLine[i], '/');
						int posI = (std::stoi(pair[0]) - 1);
						int normI = (std::stoi(pair[1]) - 1);
						// add pair[0] since it is the index to the position
						indices.push_back((uint)(std::stoi(pair[0]) - 1));
						// however do not add pair[1] to indices but instead use it to add the normals
						// to the appropriate position vector in vertices2D
						/*vertices2D[posI].push_back(normals2D[normI][0]);
						vertices2D[posI].push_back(normals2D[normI][1]);
						vertices2D[posI].push_back(normals2D[normI][2]);*/

					}
				}
			}
			// close the object file stream
			objStream.close();
			// now let's flatten vertices2D into vertices
			std::vector<float> vertices;
			for (const std::vector<float>& vec : vertices2D) {

				std::cout << vec << "\n";
				for (const float& fl : vec) {

					vertices.push_back(fl);
				}
			}
			std::cout << "\n\n\n";
			for (int i = 0; i < indices.size()-3; i += 3) {

				std::cout << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << "\n";
			}

			// return the pair
			return {vertices, indices};

		}
		void Update(float deltaTime, const glm::vec4& cameraPos);
		void Render();
		~Mesh();
	};
}

