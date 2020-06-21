#include "pch.h"
#include "Mesh.h"

#pragma region Constructor

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::shared_ptr<VkBuffer> vertexBuffer, uint32_t vertexBufferOffset, std::shared_ptr<VkBuffer> indexBuffer, uint32_t indexBufferOffset)
{
	this->vertices = vertices;
	this->indices = indices;
	this->vertexBuffer = vertexBuffer;
	this->vertexBufferOffset = vertexBufferOffset;
	this->indexBuffer = indexBuffer;
	this->indexBufferOffset = indexBufferOffset;

	transform = std::make_shared<Transform>();
}

#pragma endregion

#pragma region Accessors

std::vector<Vertex> Mesh::GetVertices()
{
	return vertices;
}

void Mesh::SetVertices(std::vector<Vertex> value)
{
	vertices = value;
}

std::shared_ptr<VkBuffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

uint32_t Mesh::GetVertexBufferOffset()
{
	return vertexBufferOffset;
}

void Mesh::SetVertexBuffer(std::shared_ptr<VkBuffer> value, uint32_t offset)
{
	vertexBuffer = value;
	vertexBufferOffset = offset;
}

std::vector<uint16_t> Mesh::GetIndices()
{
	return indices;
}

void Mesh::SetIndices(std::vector<uint16_t> value)
{
	indices = value;
}

std::shared_ptr<VkBuffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

uint32_t Mesh::GetIndexBufferOffset()
{
	return indexBufferOffset;
}

void Mesh::SetIndexBuffer(std::shared_ptr<VkBuffer> value, uint32_t offset)
{
	indexBuffer = value;
	indexBufferOffset = offset;
}

std::shared_ptr<Transform> Mesh::GetTransform()
{
	return transform;
}

#pragma endregion

#pragma region Mesh Generation

void Mesh::GeneratePlane()
{
	//Set vertices
	vertices.resize(4);

	vertices = {
		{{-0.5f, 0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
		{{0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
		{{-0.5f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
	};

	//Set indices
	indices.resize(6);

	indices = {
		0, 1, 2,
		2, 3, 0
	};

	//TODO Update Buffers
}

void Mesh::GenerateCube() {
	//Set vertices
	vertices.resize(4);

	vertices = {
		{{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
	};

	//Set indices
	indices.resize(6);

	indices = {
		0,5,1,
		0,4,5,
		2,3,7,
		2,7,6,
		1,2,0,
		1,3,2,
		4,0,2,
		4,2,6,
		5,4,6,
		5,6,7,
		5,3,1,
		5,7,3
	};

	//TODO: Update Buffers
}

#pragma endregion