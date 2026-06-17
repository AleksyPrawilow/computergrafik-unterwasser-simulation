#include "shaderLoader.h"
#include<iostream>
#include<fstream>
#include<vector>

using namespace Kern;

shaderLoader::shaderLoader()= default;
shaderLoader::~shaderLoader()= default;

std::string shaderLoader::ReadShader(const char *filename) {
	std::string shaderCode;
	std::ifstream file(filename, std::ios::in);

	if (!file.is_open() || !file.good()) {
		std::cout << "Can't read file " << filename << std::endl;
		std::terminate();
	}

	std::string filepathStr(filename);
	size_t lastSlash = filepathStr.find_last_of("/\\");
	std::string directory = (lastSlash != std::string::npos) ? filepathStr.substr(0, lastSlash + 1) : "";

	std::string line;
	while (std::getline(file, line)) {
		if (line.rfind("#include \"", 0) == 0) {
			size_t firstQuote = line.find('\"');
			size_t lastQuote = line.find('\"', firstQuote + 1);

			if (firstQuote != std::string::npos && lastQuote != std::string::npos) {
				std::string includeFileName = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
				std::string includePath = directory + includeFileName;

				// Recursively read the included file's code and append it
				shaderCode += ReadShader(includePath.c_str()) + "\n";
			}
		} else {
			shaderCode += line + "\n";
		}
	}

	file.close();
	return shaderCode;
}

GLuint shaderLoader::CreateShader(const GLenum shaderType, const std::string& source, const char* shaderName) {
	int compile_result = 0;

	const GLuint shader = glCreateShader(shaderType);
	const char *shader_code_ptr = source.c_str();
	const int shader_code_size = source.size();

	glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

	if (compile_result == GL_FALSE) {
		int info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> shader_log(info_log_length);
		glGetShaderInfoLog(shader, info_log_length, NULL, &shader_log[0]);
		std::cout << "ERROR compiling shader: " << shaderName << std::endl << &shader_log[0] << std::endl;
		return 0;
	}

	return shader;
}

GLuint shaderLoader::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
	//wczytaj shadery
	std::string vertex_shader_code = ReadShader(vertexShaderFilename);
	std::string fragment_shader_code = ReadShader(fragmentShaderFilename);

	GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code, "vertex shader");
	GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code, "fragment shader");

	int link_result = 0;
	//stworz shader
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	//sprawdz bledy w linkerze
	if (link_result == GL_FALSE) {
		int info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> program_log(info_log_length);
		glGetProgramInfoLog(program, info_log_length, NULL, &program_log[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
		return 0;
	}

	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}

void shaderLoader::DeleteProgram( GLuint program ) {
	glDeleteProgram(program);
}
