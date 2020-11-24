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
#include <glm.hpp>

typedef unsigned int uint;
typedef unsigned char uchar;

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

	// class for abstracting gl textures away (meant to be used inside a vertex array class)
	// a vao has to be bound in order to use which is why you should just use it inside a vao class
	// assumes a 2d texture file
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
			std::cout << imageSize << "\n";
			// Read the actual data from the file into the buffer
			fread(imgBytes, 1, imageSize, imgBuffer);

			//Everything is in memory now, the file can be closed
			fclose(imgBuffer);
			return { imgBytes, {width, height} };
		}

	public:

		Texture(const char* bmpFile, const std::vector<std::pair<uint, uint>>& paramPairs);
	};

	// object for containing a vertex array object as well as vertex buffer data
	class VertexArray {

		// vao id
		uint vao;
		// vbo ids (positions and indices respectively)
		uint vbo[2];
		// indices size
		uint indicesSize;
		// texture is allocated on the heap
		Texture* texture;

	public:

		// vertex array for mesh without texture
		VertexArray(std::vector<float> vertices, std::vector<uint> indices);
		// vertex array for mesh with texture (for vertices expecting 3 floats for pos, 3 floats for color, and 2 floats for texture coords per point)
		VertexArray(std::vector<float> vertices, std::vector<uint> indices, const char* bmpFile, const std::vector<std::pair<uint, uint>>& paramPairs);
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

		VEC2 = 2,
		VEC3 = 3,
		VEC4 = 4,
		MAT4 = 16
	};

	// shader program class
	class Program {

		// program id
		uint program;

	public:

		// pass in a vector of shaderFileNames with their corresponding enums (ie {{"myVertexShader.glsl", GL_VERTEX_SHADER},...})
		Program(const std::vector<std::pair<const char*, uint>>& shaderFileNameEnumPairs);
		// compile an individual shader
		uint CompileShader(const char* shaderFileName, uint shaderEnum);
		// link the entire program
		void LinkProgram(const std::vector<uint>& shaderIDs);
		inline void Use() { glUseProgram(program); }
		inline void Unuse() { glUseProgram(0); }
		void SetUniformValue(const char* uniformName, float* dataBegin, uint dataTypeEnum);
		// returns a basic shader that colors a shape one color
		static Program Default() {

			return Program({ {"shaders/default_vert.glsl", bndr::VERT_SHDR}, {"shaders/default_frag.glsl", bndr::FRAG_SHDR} });
		}
		// returns a shader program for textured objects
		static Program Textured() {

			return Program({ {"shaders/texture_vert.glsl", bndr::VERT_SHDR}, {"shaders/texture_frag.glsl", bndr::FRAG_SHDR} });
		}
	};

	class Mesh {



	public:
	};
}

