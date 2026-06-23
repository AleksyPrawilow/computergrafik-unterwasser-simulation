#pragma once
#include "glm.hpp"
#include "glew.h"
#include "objload.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <unordered_map>
#include <string>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Kern
{
	struct RenderContext {
		GLuint vertexArray{};
		GLuint vertexBuffer{};
		GLuint vertexIndexBuffer{};
		GLuint instanceVBO = 0;
		int size = 0;
		void initFromAssimpMesh(const aiMesh* mesh);
		void setupInstanceBuffer(const std::vector<glm::mat4>& matrices);
	};

	struct VertexAttribute {
		const void * Pointer;
		int Size;
	};

	struct VertexData {
		static constexpr int MAX_ATTRIBS = 8;
		VertexAttribute Attribs[MAX_ATTRIBS];
		int NumActiveAttribs;
		int NumVertices;
	};

	static std::unordered_map<GLuint, std::unordered_map<std::string, GLint>> uniformCache;

	void DrawVertexArray(const float * vertexArray, int numVertices, int elementSize);
	void DrawVertexArrayIndexed(const float * vertexArray, const int * indexArray, int numIndexes, int elementSize);
	void DrawVertexArray(const VertexData & data);
	void DrawContext(const RenderContext& context);

	glm::vec2 GetViewportSize();
	glm::mat4 GetOrthoProjection();
	void Set2DRenderState(bool enable);
	void DrawQuad(GLuint vao);

	void SetBlendState(bool enable);
	void SetDepthWriteState(bool enable);
	void SetCullState(bool enable);

	GLint getUniformLocation(GLuint program, const char * name);
	void clearUniformCache();

	void setUniform(GLint location, int value);
	void setUniform(GLint location, bool value);
	void setUniform(GLint location, float value);
	void setUniform(GLint location, const glm::vec2& value);
	void setUniform(GLint location, const glm::vec3& value);
	void setUniform(GLint location, const glm::vec4& value);
	void setUniform(GLint location, const glm::mat3& value);
	void setUniform(GLint location, const glm::mat4& value);

	void setUniform(GLuint program, const char* name, int value);
	void setUniform(GLuint program, const char* name, bool value);
	void setUniform(GLuint program, const char* name, float value);
	void setUniform(GLuint program, const char* name, const glm::vec2& value);
	void setUniform(GLuint program, const char* name, const glm::vec3& value);
	void setUniform(GLuint program, const char* name, const glm::vec4& value);
	void setUniform(GLuint program, const char* name, const glm::mat3& value);
	void setUniform(GLuint program, const char* name, const glm::mat4& value);
}