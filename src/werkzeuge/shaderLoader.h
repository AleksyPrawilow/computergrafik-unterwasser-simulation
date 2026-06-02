#pragma once

#include "glew.h"
#include "freeglut.h"
#include <iostream>

namespace Core {
	class shaderLoader {
	private:
		static std::string ReadShader(const char *filename);
		static GLuint CreateShader(GLenum shaderType,
			const std::string& source,
			const char* shaderName);

	public:
		shaderLoader();
		~shaderLoader();
		static GLuint CreateProgram(const char* VertexShaderFilename,
			const char* FragmentShaderFilename);

		void DeleteProgram(GLuint program);

	};
}