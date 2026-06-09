#pragma once
#include "glm.hpp"
#include "glew.h"
#include "objload.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Kern
{
	struct RenderContext {
		GLuint vertexArray{};
		GLuint vertexBuffer{};
		GLuint vertexIndexBuffer{};
		int size = 0;
		void initFromAssimpMesh(aiMesh* mesh);
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
}