#pragma once
#include "Vector3.h"
class camera
{
public:
	camera();
	~camera();
	void update();

	Vector3 GetPosition();
	Vector3 GetForward();
	Vector3 GetUp();
	Vector3 GetLookAt();

	void SetPosition(Vector3 newPosition);

	void SetLookAt(Vector3 newLookAt);

	void MoveForward(float dt);
	void MoveBackwards(float dt);
	void MoveRight(float dt);
	void MoveLeft(float dt);
	/*void LookUp(float dt);
	void LookDown(float dt);
	void LookRight(float dt);
	void LookLeft(float dt);*/
	void RotateYaw(float val, float dt, float sens);
	void RotatePitch(float val, float dt, float sens);

	void lookAtCenter();

private:
	float Yaw, Pitch, Roll;

	float speed;

	Vector3 position;
	Vector3 lookAt;
	Vector3 forward;
	Vector3 up;
	Vector3 right;
};

