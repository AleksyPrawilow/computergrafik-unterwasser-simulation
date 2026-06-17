#pragma once
 
#include "glew.h"
#include <vector>
#include <string>
#include "freeglut.h"

namespace Kern
{
	GLuint LoadTexture(const char * filepath, bool pixelArt = false);
	GLuint LoadCubemap(const std::vector<std::string>& faces);
	void SetActiveTexture(GLuint textureID, const char * shaderVariableName, GLuint programID, int textureUnit);
}