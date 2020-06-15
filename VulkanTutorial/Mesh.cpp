#include "pch.h"
#include "Mesh.h"

#pragma region Constructor

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices)
{
	this->vertices = vertices;
	this->indices = indices;

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

std::vector<uint16_t> Mesh::GetIndices()
{
	return indices;
}

void Mesh::SetIndices(std::vector<uint16_t> value)
{
	indices = value;
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
		{{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},
		{{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
	};

	//Set indices
	indices.resize(6);

	indices = { 0, 1, 2, 2, 3, 0 };
}

#pragma endregion