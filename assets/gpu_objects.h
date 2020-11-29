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
#include <gtc/matrix_transform.hpp>
#include <glm.hpp>

typedef unsigned int uint;
typedef unsigned char uchar;

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

		// vertex array (numtextures by default is zero) if numTextures > 0 then the stride will be:
		// x y z r g b a u v i = 3 pos floats + 4 color floats + 2 texture floats + 1 texture index =
		// (3+4+2+1)*sizeof(float) = 10 * sizeof(float)
		// otherwise if numTextures = 0 then stride is just 7 * sizeof(float) (x y z r g b a)
		VertexArray(std::vector<float> vertices, std::vector<uint> indices, bool usingTextures = false);
		// update the buffer data with new vertices and indices
		void UpdateBufferData(std::vector<float> vertices, std::vector<uint> indices);
		// load a texture to use with the vao (make sure the vertices specify color coords and texture coords)
		inline void Bind() { glBindVertexArray(vao); }
		inline void Unbind() { glBindVertexArray(0); }
		void Render();
		~VertexArray();
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
		// create a program for a tetrimino
		static Program BasicModel() {

			return Program({ {"shaders/tetrimino_shdr/tetrimino_vert.glsl", bndr::VERT_SHDR}, {"shaders/tetrimino_shdr/tetrimino_frag.glsl", bndr::FRAG_SHDR} });
		}
	};

	// mesh that contains vertices, color, and can load it all from a blender object file
	class Mesh {

		VertexArray* vao;
		Program program;

	public:

		Mesh(const char* objFile, uint shaderProgramType, std::vector<float> color = {1.0f, 1.0f, 1.0f, 1.0f});
		// methods to update uniforms
		void Translate(float xTrans, float yTrans, float zTrans);
		void Rotate(float angle, const std::vector<uint>& axes, const glm::vec3& center = { 0.0f, 0.0f, 0.0f });
		void Scale(float xScale, float yScale, float zScale);

		void CameraView(const glm::mat4x4& cameraMat);
		void Project(const glm::mat4x4& perspective);
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

			std::vector<float> vertices;
			std::vector<uint> indices;
			std::string line;
			while (std::getline(objStream, line)) {

				// these are the position vertices
				if (line[0] == 'v' && line[1] == ' ') {

					// split up the data in each line
					std::vector<std::string> splitLine = SplitStr(line, ' ');
					// we skip the v so we set i to 1
					for (int i = 1; i < splitLine.size(); i++) {

						vertices.push_back(std::stof(splitLine[i]));
					}
					// now add the default color if the object file doesn't specify it
					if (!objectHasColors) {

						for (float& fl : color) {

							vertices.push_back(fl);
						}
					}
				}
				// faces (indices to vertices)
				else if (line[0] == 'f' && line[1] == ' ') {

					// replace slashes with a space
					ReplaceStrChar(line, '/', ' ');
					std::vector<std::string> splitLine = SplitStr(line, ' ');
					// get the indices whilst skipping f
					for (int i = 1; i < splitLine.size(); i++) {

						indices.push_back((uint)std::stoi(splitLine[i]));
					}
				}
			}
			// close the object file stream
			objStream.close();
			// return the pair
			return {vertices, indices};

		}
		~Mesh();
	};
}

