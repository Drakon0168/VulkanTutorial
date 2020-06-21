#pragma once

#include "pch.h"
#include "Vertex.h"
#include "Transform.h"

class Mesh
{
private:
	std::vector<Vertex> vertices;
	uint32_t vertexBufferOffset;
	std::shared_ptr<VkBuffer> vertexBuffer;

	std::vector<uint16_t> indices;
	uint32_t indexBufferOffset;
	std::shared_ptr<VkBuffer> indexBuffer;

	std::shared_ptr<Transform> transform;

public:

#pragma region Constructor

	Mesh(
		std::vector<Vertex> vertices = {},
		std::vector<uint16_t> indices = {},
		std::shared_ptr<VkBuffer> vertexBuffer = nullptr, uint32_t vertexBufferOffset = 0, 
		std::shared_ptr<VkBuffer> indexBuffer = nullptr, uint32_t indexBufferOffset = 0);

#pragma endregion

#pragma region Accessors

	/// <summary>
	/// Returns the list of vertices associated with this mesh
	/// </summary>
	/// <returns>List of vertices</returns>
	std::vector<Vertex> GetVertices();

	/// <summary>
	/// Sets the list of vertices associated with this mesh
	/// </summary>
	/// <param name="value">The list to set vertices to</param>
	void SetVertices(std::vector<Vertex> value);

	/// <summary>
	/// Returns a shared pointer to the vertex buffer with this mesh's data
	/// </summary>
	/// <returns>The vertex buffer with this mesh's data</returns>
	std::shared_ptr<VkBuffer> GetVertexBuffer();

	/// <summary>
	/// The offset in the vertex buffer for this mesh's data
	/// </summary>
	/// <returns>The vertex buffer offset</returns>
	uint32_t GetVertexBufferOffset();

	/// <summary>
	/// Sets the vertex buffer and vertex offset 
	/// </summary>
	/// <param name="value">The vertex buffer to set to</param>
	/// <param name="offset">The offset within the buffer that this mesh's data is stored at</param>
	void SetVertexBuffer(std::shared_ptr<VkBuffer> value, uint32_t offset = 0);

	/// <summary>
	/// Returns the list of indices associated with this mesh
	/// </summary>
	/// <returns>List of indices</returns>
	std::vector<uint16_t> GetIndices();

	/// <summary>
	/// Sets the list of indices associated with this mesh
	/// </summary>
	/// <param name="value">The list to set indices to</param>
	void SetIndices(std::vector<uint16_t> value);

	/// <summary>
	/// Returns a shared pointer to the index buffer with this mesh's data
	/// </summary>
	/// <returns>The index buffer with this mesh's data</returns>
	std::shared_ptr<VkBuffer> GetIndexBuffer();

	/// <summary>
	/// The offset in the index buffer for this mesh's data
	/// </summary>
	/// <returns>The index buffer offset</returns>
	uint32_t GetIndexBufferOffset();

	/// <summary>
	/// Sets the index buffer and index offset
	/// </summary>
	/// <param name="value">The index buffer to set to</param>
	/// <param name="offset">The offset within the buffer that this mesh's data is stored at</param>
	void SetIndexBuffer(std::shared_ptr<VkBuffer> value, uint32_t offset = 0);

	/// <summary>
	/// Returns the transform associated with this object
	/// </summary>
	/// <returns>The transform attached to this object</returns>
	std::shared_ptr<Transform> GetTransform();

#pragma endregion

#pragma region Mesh Generation

	/// <summary>
	/// Sets the vertices and indices to generate a plane
	/// </summary>
	void GeneratePlane();

	/// <summary>
	/// Sets the vertices and indices to generate a cube
	/// </summary>
	void GenerateCube();

#pragma endregion
};