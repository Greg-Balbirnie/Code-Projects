#include "GameObject.h"

GameObject::GameObject()
{
	MeshInstance();
}  

GameObject::~GameObject()
{
}

gef::Vector4 GameObject::Get_Position()
{
	return transform_.GetTranslation();
}

void GameObject::Set_Position(gef::Vector4 new_position)
{
	transform_.SetTranslation(new_position);
}

void GameObject::Set_Rotation(float rot_x, float rot_y, float rot_z)
{

	rotation_x = rot_x;
	rotation_y = rot_y;
	rotation_z = rot_z;

	Build_Transform();
}

gef::Vector4 GameObject::Get_Scale()
{
	return transform_.GetScale();
}

void GameObject::Set_Scale(gef::Vector4 new_scale)
{
	transform_.Scale(new_scale);
}

void GameObject::set_transform(gef::Matrix44 new_transform)
{
	transform_ = new_transform;
}

void GameObject::Build_Transform()
{
	// Create Matrices
	gef::Matrix44 translation_matrix;
	gef::Matrix44 rotation_matrix_x;
	gef::Matrix44 rotation_matrix_y;
	gef::Matrix44 rotation_matrix_z;
	gef::Matrix44 scale_matrix;

	// Set to identity
	translation_matrix.SetIdentity();
	rotation_matrix_x.SetIdentity();
	rotation_matrix_y.SetIdentity();
	rotation_matrix_z.SetIdentity();
	scale_matrix.SetIdentity();

	// Set all matrices
	translation_matrix.SetTranslation(transform_.GetTranslation());
	rotation_matrix_x.RotationX(rotation_x);
	rotation_matrix_y.RotationX(rotation_y);
	rotation_matrix_z.RotationX(rotation_z);
	scale_matrix.Scale(transform_.GetScale());

	// set the transform
	transform_ = scale_matrix * rotation_matrix_x *  rotation_matrix_y * rotation_matrix_z * translation_matrix;
}
