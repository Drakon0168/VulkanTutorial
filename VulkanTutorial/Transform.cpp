#include "pch.h"
#include "Transform.h"

#pragma region Constructor

Transform::Transform(glm::vec3 postition, glm::quat orientation, glm::vec3 scale)
{
	//Set position rotation and scale
	this->position = postition;
	this->orientation = orientation;
	this->scale = scale;

	//Generate model matrix
	model = {};
	isDirty = true;
	GenerateModelMatrix();
}

#pragma endregion

#pragma region Accessors

glm::vec3 Transform::GetPosition()
{
	return position;
}

void Transform::SetPosition(glm::vec3 value)
{
	position = value;

	//Mark model matrix for regeneration
	isDirty = true;
}

glm::quat Transform::GetOrientation()
{
	return orientation;
}

void Transform::SetOrientation(glm::quat value)
{
	orientation = value;

	//Mark the model matrix for regeneration
	isDirty = true;
}

void Transform::SetOrientation(glm::vec3 value, bool degrees)
{
	if (degrees) {
		value *= DEGTORAD;
	}

	orientation = glm::quat(value);

	//Mark the model matrix for regeneration
	isDirty = true;
}

glm::vec3 Transform::GetScale()
{
	return scale;
}

void Transform::SetScale(glm::vec3 value)
{
	scale = value;

	//Mark the model matrix for regeneration
	isDirty = true;
}

glm::mat4 Transform::GetModelMatrix()
{
	//Generate the model matrix if it has been changed
	if (isDirty) {
		GenerateModelMatrix();
	}

	return model;
}

#pragma endregion

#pragma region Transformations

void Transform::Translate(glm::vec3 translation, bool localSpace)
{
	//If we are moving in local space we must rotate the translation by the transforms orientation
	if (localSpace) {
		translation = orientation * translation;
	}

	//Update position by translation
	position += translation;

	//Mark the model matrix for regeneration
	isDirty = true;
}

void Transform::Rotate(glm::quat rotation)
{
	orientation = rotation * orientation;

	//Mark the model matrix for regeneration
	isDirty = true;
}

void Transform::Rotate(glm::vec3 eulerRotation, bool degrees)
{
	//If the rotation is in degrees convert to radians for glm to work with it
	if (degrees) {
		eulerRotation *= DEGTORAD;
	}

	//Rotate the orientation
	Rotate(glm::quat(eulerRotation));
}

void Transform::GenerateModelMatrix()
{
	//Update the matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotationMatrix = glm::mat4(orientation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	model = translationMatrix * rotationMatrix * scaleMatrix;

	//Mark the matrix as clean
	isDirty = false;
}

#pragma endregion