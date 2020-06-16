#pragma once

#include "pch.h"
#include "Vertex.h"
#include "Transform.h"

class Mesh
{
private:
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	std::shared_ptr<Transform> transform;

public:

#pragma region Constructor

	Mesh(std::vector<Vertex> vertices = {}, std::vector<uint16_t> indices = {});

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