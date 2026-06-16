#include "renderWerkzeuge.h"
#include <algorithm>
#include "GL/glew.h"
#include <assimp/scene.h>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

const int Kern::VertexData::MAX_ATTRIBS;
static std::unordered_map<GLuint, std::unordered_map<std::string, GLint>> uniformCache;

void Kern::RenderContext::initFromAssimpMesh(const aiMesh* mesh) {
    vertexArray = 0;
    vertexBuffer = 0;
    vertexIndexBuffer = 0;

    std::vector<float> textureCoord;
    std::vector<unsigned int> indices;
    //tex coord must be converted to 2d vecs
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        if (mesh->mTextureCoords[0] != nullptr) {
            textureCoord.push_back(mesh->mTextureCoords[0][i].x);
            textureCoord.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            textureCoord.push_back(0.0f);
            textureCoord.push_back(0.0f);
        }
    }
    if (mesh->mTextureCoords[0] == nullptr) {
        std::cout << "no uv coords\n";
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    unsigned int vertexDataBufferSize = sizeof(float) * mesh->mNumVertices * 3;
    unsigned int vertexNormalBufferSize = sizeof(float) * mesh->mNumVertices * 3;
    unsigned int vertexTexBufferSize = sizeof(float) * mesh->mNumVertices * 2;
    unsigned int vertexTangentBufferSize = sizeof(float) * mesh->mNumVertices * 3;
    unsigned int vertexBiTangentBufferSize = sizeof(float) * mesh->mNumVertices * 3;

    unsigned int vertexElementBufferSize = sizeof(unsigned int) * indices.size();
    size = indices.size();

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);


    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexElementBufferSize, &indices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    //std::cout << vertexBuffer;
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glBufferData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize + vertexTangentBufferSize + vertexBiTangentBufferSize, NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataBufferSize, mesh->mVertices);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize, vertexNormalBufferSize, mesh->mNormals);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize, vertexTexBufferSize, &textureCoord[0]);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize, vertexTangentBufferSize, mesh->mTangents);

    glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize + vertexTangentBufferSize, vertexBiTangentBufferSize, mesh->mBitangents);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertexNormalBufferSize + vertexDataBufferSize));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize + vertexTangentBufferSize));

}

void Kern::DrawVertexArray(const float * vertexArray, int numVertices, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Kern::DrawVertexArrayIndexed( const float * vertexArray, const int * indexArray, int numIndexes, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, indexArray);
}


void Kern::DrawVertexArray( const VertexData & data )
{
	int numAttribs = std::min(VertexData::MAX_ATTRIBS, data.NumActiveAttribs);
	for(int i = 0; i < numAttribs; i++)
	{
		glVertexAttribPointer(i, data.Attribs[i].Size, GL_FLOAT, false, 0, data.Attribs[i].Pointer);
		glEnableVertexAttribArray(i);
	}
	glDrawArrays(GL_TRIANGLES, 0, data.NumVertices);
}

void Kern::DrawContext(const Kern::RenderContext& context)
{

	glBindVertexArray(context.vertexArray);
	glDrawElements(
		GL_TRIANGLES,      // mode
		context.size,    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
	glBindVertexArray(0);
}

glm::vec2 Kern::GetViewportSize() {
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	return glm::vec2(static_cast<float>(viewport[2]), static_cast<float>(viewport[3]));
}

glm::mat4 Kern::GetOrthoProjection() {
	const glm::vec2 size = GetViewportSize();
	return glm::ortho(0.0f, size.x, size.y, 0.0f, -1.0f, 1.0f);
}

void Kern::Set2DRenderState(const bool enable) {
	if (enable) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}

void Kern::DrawQuad(const GLuint vao) {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Kern::SetBlendState(const bool enable) {
	if (enable) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}
}

void Kern::SetDepthWriteState(const bool enable) {
	glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

void Kern::SetCullState(const bool enable) {
	if (enable) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

GLint Kern::getUniformLocation(const GLuint program, const char* name) {
	auto& progCache = uniformCache[program];
	const std::string nameStr(name);

	if (const auto it = progCache.find(nameStr); it != progCache.end()) {
		return it->second;
	}

	const GLint location = glGetUniformLocation(program, name);
	progCache[nameStr] = location;
	return location;
}

void Kern::clearUniformCache() {
	uniformCache.clear();
}

void Kern::setUniform(const GLint location, const int value) {
	if (location != -1) glUniform1i(location, value);
}
void Kern::setUniform(const GLint location, const bool value) {
	if (location != -1) glUniform1i(location, value ? 1 : 0);
}
void Kern::setUniform(const GLint location, const float value) {
	if (location != -1) glUniform1f(location, value);
}
void Kern::setUniform(const GLint location, const glm::vec2& value) {
	if (location != -1) glUniform2fv(location, 1, glm::value_ptr(value));
}
void Kern::setUniform(const GLint location, const glm::vec3& value) {
	if (location != -1) glUniform3fv(location, 1, glm::value_ptr(value));
}
void Kern::setUniform(const GLint location, const glm::vec4& value) {
	if (location != -1) glUniform4fv(location, 1, glm::value_ptr(value));
}
void Kern::setUniform(const GLint location, const glm::mat3& value) {
	if (location != -1) glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}
void Kern::setUniform(const GLint location, const glm::mat4& value) {
	if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Kern::setUniform(const GLuint program, const char* name, const int value) {
	setUniform(getUniformLocation(program, name), value);
}
void Kern::setUniform(const GLuint program, const char* name, const bool value) {
	setUniform(getUniformLocation(program, name), value);
}
void Kern::setUniform(const GLuint program, const char* name, const float value) {
	setUniform(getUniformLocation(program, name), value);
}
void Kern::setUniform(const GLuint program, const char* name, const glm::vec2& value) {
	setUniform(getUniformLocation(program, name), value);
}
void Kern::setUniform(const GLuint program, const char* name, const glm::vec3& value) {
	setUniform(getUniformLocation(program, name), value);
}
void Kern::setUniform(const GLuint program, const char* name, const glm::vec4& value) {
	setUniform(getUniformLocation(program, name), value);
}
void Kern::setUniform(const GLuint program, const char* name, const glm::mat3& value) {
	setUniform(getUniformLocation(program, name), value);
}
void Kern::setUniform(const GLuint program, const char* name, const glm::mat4& value) {
	setUniform(getUniformLocation(program, name), value);
}