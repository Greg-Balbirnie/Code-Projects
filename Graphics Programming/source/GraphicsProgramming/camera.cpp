#include "camera.h"
#include<math.h>

camera::camera()
{
	//default values
	position.set(0, 0, 6);
	lookAt.set(0, 0, 0);
	speed = 0.1;

	Yaw = 0;
	Pitch = 0;
	Roll = 0;
}

camera::~camera()
{
}

void camera::update()
{

	float cosR, cosP, cosY;	//temp values for sin/cos from 
	float sinR, sinP, sinY;
	// Roll, Pitch and Yall are variables stored by the camera
	// handle rotation
	// Only want to calculate these values once, when rotation changes, not every frame. 
	cosY = cosf(Yaw*3.1415 / 180);
	cosP = cosf(Pitch*3.1415 / 180);
	cosR = cosf(Roll*3.1415 / 180);
	sinY = sinf(Yaw*3.1415 / 180);
	sinP = sinf(Pitch*3.1415 / 180);
	sinR = sinf(Roll*3.1415 / 180);
	//This using the parametric equation of a sphere

	// Calculate the three vectors to put into  glu Lookat
	// Look direction,  position and the up vector 
	// This function could also calculate the right vector

	forward.x = sinY * cosP;
	forward.y = sinP;
	forward.z = cosP * -cosY;
	// Look At Point
	// To calculate add Forward Vector to Camera position.	

	// Up Vector
	up.x = -cosY * sinR - sinY * sinP * cosR;
	up.y = cosP * cosR;
	up.z = -sinY * sinR - sinP * cosR * -cosY;

	// Side Vector (right)
	right = up.cross(forward);

	//find lookat
	lookAt = position + forward;
}

Vector3 camera::GetForward()
{
	return forward;
}

Vector3 camera::GetPosition()
{
	return position;
}

Vector3 camera::GetUp()
{
	return up;
}

Vector3 camera::GetLookAt()
{
	return lookAt;
}

void camera::SetPosition(Vector3 newPosition)
{
	position = newPosition;
}

void camera::SetLookAt(Vector3 newLookAt)
{
	lookAt = newLookAt;
}

//multiply the direction vector by the speed and then add it to the position 
void camera::MoveForward(float dt)
{
	forward.x = forward.x * speed;
	forward.y = forward.y * speed;
	forward.z = forward.z * speed;

	position.x += forward.x;
	position.y += forward.y;
	position.z += forward.z;
	update();
}

void camera::MoveBackwards(float dt)
{
	forward.x = forward.x * -speed;
	forward.y = forward.y * -speed;
	forward.z = forward.z * -speed;

	position.x += forward.x;
	position.y += forward.y;
	position.z += forward.z;
	update();
}

void camera::MoveLeft(float dt)
{
	right.x = right.x * speed;
	right.y = right.y * speed;
	right.z = right.z * speed;

	position.x += right.x;
	position.y += right.y;
	position.z += right.z;
	update();
}

void camera::MoveRight(float dt)
{
	right.x = right.x * -speed;
	right.y = right.y * -speed;
	right.z = right.z * -speed;

	position.x += right.x;
	position.y += right.y;
	position.z += right.z;
	update();
}

//takes movement of mouse and then lowers its sensitivity
void camera::RotateYaw(float val, float dt, float sens)
{
	Yaw += val * sens * 0.1;
	update();
}
void camera::RotatePitch(float val, float dt, float sens)
{
	if (Pitch >= 90)
	{
		Pitch = 90;
	}
	if (Pitch <= -90)
	{
		Pitch = -90;
	}
	Pitch += val * sens * 0.1;
	update();
}