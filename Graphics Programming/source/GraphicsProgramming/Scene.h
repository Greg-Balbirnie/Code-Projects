// Scene class. Configures a basic 3D scene.
// Interfaces with the Input class to handle user input
// Calculates and outputs Frames Per Second (FPS) rendered.
// Important functions are the constructor (initialising the scene), 
// update (for process user input and updating scene objects) and render (renders scene).
#ifndef _SCENE_H
#define _SCENE_H

// Include GLUT, openGL, input.
#include "glut.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Input.h"
#include <stdio.h>
// Further includes should go here:
#include"camera.h"
#include"Model.h"
#include"Shape.h"
class Scene {

public:
	Scene(Input *in);
	// Main render function
	void render();
	void renderObjects();
	void generateShadowMatrix(float light_pos[4], GLfloat floor[12]);
	void spawnCard(float rotateSpeed, float height, float distance);
	// Update function receives delta time from parent (used for frame independent updating).
	void update(float dt);
	// Resizes the OpenGL output based on new window size.
	void resize(int w, int h);

protected:
	// Renders text (x, y positions, RGB colour of text, string of text to be rendered)
	void displayText(float x, float y, float r, float g, float b, char* string);
	// A function to collate all text output in a single location
	void renderTextOutput();

	// For access to user input.
	Input* input;

	// For Window and frustum calculation.
	int width, height;
	float fov, nearPlane, farPlane;

	// For FPS counter and mouse coordinate output.
	int frame = 0, time, timebase = 0;
	char fps[12];
	char mouseText[20];

	//added stuff
	camera* currentCamera;
	camera mainCamera;
	camera birdCamera;

	int centre[2] = { width / 2, height / 2 };

	Model teapot;
	Model swan;
	Model table;
	Model plate;
	Model bottle;
	Model bottlePt2;
	Model cup;

	GLuint checkerTex;
	GLuint sky;
	GLuint cardTex;
	GLuint ice;

	int cardRotation;
	int plateRotation;
	Shape s;
	bool wireframe;
	int worldScale;
	bool shadowPass;

	float shadowMatrix[16];
};

#endif