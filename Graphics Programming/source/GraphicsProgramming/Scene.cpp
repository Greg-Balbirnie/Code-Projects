#include "Scene.h"
#include"camera.h"
#include "Model.h"
#include "Shape.h"

Scene::Scene(Input *in)
{
	// Store pointer for input class
	input = in;

	//OpenGL settings
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.39f, 0.58f, 93.0f, 1.0f);			// Cornflour Blue Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glClearStencil(0);									// clear stencil buffer
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_COLOR_MATERIAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Other OpenGL / render setting should be applied here.
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_NORMALIZE);

	// Initialise variables
	currentCamera = &mainCamera;			//set the first camera to the controllable camera
	currentCamera->update();				//update the camera
	cardRotation = 0;						//initalise card rotation
	plateRotation = 0;						//initialise plate rotation
	wireframe = false;						//set wireframe mode off
	shadowPass = false;						//true if its passing through the code to render shadows

	Vector3 birdPos;						//declare position of birds eye camera
	birdPos.setX(0);						//set X
	birdPos.setY(15);						//set Y
	birdPos.setZ(5);						//set Z
	birdCamera.SetPosition(birdPos);		//set the position of the camera

	checkerTex = SOIL_load_OGL_texture		//checkered floor texture
		(
			"textures/checker.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);

	sky = SOIL_load_OGL_texture				//skybox texture
		(
			"textures/skybox.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);

	cardTex = SOIL_load_OGL_texture			//card texture
		(
			"textures/card.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);

	ice = SOIL_load_OGL_texture				//ice texture
		(
			"textures/ice.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
			);

	//Load Models
	teapot.load("models/MyTeapot.obj", "textures/teapot.png");
	swan.load("models/swan.obj", "textures/ice.png");
	table.load("models/table.obj", "textures/table.png");
	plate.load("models/plate.obj", "textures/plate.png");
	bottle.load("models/bottle.obj", "textures/DrinkMe.png");
	bottlePt2.load("models/bottlePt2.obj", "textures/DrinkMe.png");
	cup.load("models/cup.obj", "textures/cup.png");
}

void Scene::update(float dt)
{
	// Handle user input
	// Update object and variables (camera, rotation, etc).
	if (currentCamera == &mainCamera)
	{
		if (input->isKeyDown('w'))		//move forward with W
		{
			currentCamera->MoveForward(dt);
		}
		if (input->isKeyDown('s'))		//move back with S
		{
			currentCamera->MoveBackwards(dt);
		}
		if (input->isKeyDown('a'))		//move left with A
		{
			currentCamera->MoveLeft(dt);
		}
		if (input->isKeyDown('d'))		//move right with D
		{
			currentCamera->MoveRight(dt);
		}

		//move with cursor
		currentCamera->RotateYaw(400 - input->getMouseX(), dt, -0.5);
		currentCamera->RotatePitch(300 - input->getMouseY(), dt, 0.5);
	}
	
	//move cursor to middle and do not display it
	glutWarpPointer(400, 300);
	glutSetCursor(GLUT_CURSOR_NONE);

	if (input->isKeyDown('p'))		//P toggles wireframe
	{
		if (wireframe == false)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			input->SetKeyUp('p');
			wireframe = true;
		}
		else if (wireframe == true)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			input->SetKeyUp('p');
			wireframe = false;
		}
	}

	if (input->isKeyDown('c'))		//C toggles cameras
	{
		if (currentCamera != &birdCamera)
		{
			currentCamera = &birdCamera;
			input->SetKeyUp('c');
		}
		else if (currentCamera == &birdCamera)
		{
			currentCamera = &mainCamera;
			input->SetKeyUp('c');
		}
	}

	// Calculate FPS
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		sprintf_s(fps, "FPS: %4.2f", frame*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
	}

	//R & E spin the plates clockwise/anti-clockwise
	if (input->isKeyDown('r'))
	{
		plateRotation += 2;
	}
	if (input->isKeyDown('e'))
	{
		plateRotation -= 2;
	}

	//cards constantly rotate
	cardRotation += 1;
}

//ambient value on all lights
GLfloat Light_Ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };

//directional light
GLfloat Light_Diffuse[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat Light_Position[] = { 0.0f, 1.0f, 0.0f, 0.0f };

//bottle light
GLfloat Light_Blue[] = { 0.03f, 0.03f, 1.0f, 1.0f };
GLfloat Light_Position2[] = { 0.0f, 0.0f, 0.0f, 1.0f };

//shadow light
GLfloat Light_Shadow[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat Light_Position3[] = { 0.0f, 5.0f, 5.0f, 1.0f };

//move light
GLfloat Light_Move[] = { 1.0f, 1.0f, 0.0f, 1.0f };
GLfloat Light_Position4[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat spot_Direction[] = { 0.0f, -1.0f, 0.0f };

//vertex array of the floor for the shadows
float verts2[12] = { -5.0f, -0.99f, 0.0f,
					-5.0f, -0.99f, 10.0f,
					5.0f, -0.99f, 10.0f,
					5.0f, -0.99f, 0.0f };

void Scene::render() {

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Set the camera
	//birds eye camera has fixed values 
	//main camera can be moved
	if (currentCamera == &birdCamera)	
	{
		gluLookAt(0, 15, 5,
					0, -1, 5,
					0, 0, 1);
	}
	else
	{	
		gluLookAt(currentCamera->GetPosition().x, currentCamera->GetPosition().y, currentCamera->GetPosition().z,
		currentCamera->GetLookAt().x, currentCamera->GetLookAt().y, currentCamera->GetLookAt().z,
		currentCamera->GetUp().x, currentCamera->GetUp().y, currentCamera->GetUp().z);
	}
	
		
	//skybox
	glPushMatrix();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glTranslatef(currentCamera->GetPosition().x, currentCamera->GetPosition().y, currentCamera->GetPosition().z);		//move skybox to current camera
		glBindTexture(GL_TEXTURE_2D, sky);
		s.renderSky();
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	glPopMatrix();

	//set up mirror / real world
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);						//create mirror stencil plane
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
	glEnd();
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
		glScalef(1.0, 1.0, -1.0);
		glTranslatef(0.0, 0.0, 5.0);
		renderObjects();					//create scene in mirror
	glPopMatrix();
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
		glBegin(GL_QUADS);					//create mirror object
			glVertex3f(-1.0f, -1.0f, 0.0f);
			glVertex3f(1.0f, -1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, 0.0f);
		glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPushMatrix();
		glTranslatef(0.0, 0.0, 5.0);
		renderObjects();					//create real scene
	glPopMatrix();

	//directional lighting
	glPushMatrix();
		glLightfv(GL_LIGHT0, GL_AMBIENT, Light_Ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_Diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, Light_Position);
		glEnable(GL_LIGHT0);
	glPopMatrix();

	//set up blue light on bottle
	glPushMatrix();
		glTranslatef(1.5, -0.4, 5.0);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Blue);
		glLightfv(GL_LIGHT1, GL_POSITION, Light_Position2);
		glLightfv(GL_LIGHT1, GL_AMBIENT, Light_Ambient);
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 1);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 1);
		glEnable(GL_LIGHT1);
	glPopMatrix();
	
	//set up orange light
	glPushMatrix();
		glTranslatef(0, 2, 5.0);
		glRotatef(cardRotation, 0, 1, 0);
		glTranslatef(0, 0, 2);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, Light_Move);
		glLightfv(GL_LIGHT3, GL_POSITION, Light_Position4);
		glLightfv(GL_LIGHT3, GL_AMBIENT, Light_Ambient);
		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 45.0f);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_Direction);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 50.0);
		glEnable(GL_LIGHT3);
	glPopMatrix();

	//shadow 
	glPushMatrix();
		glLightfv(GL_LIGHT2, GL_DIFFUSE, Light_Shadow);
		glLightfv(GL_LIGHT2, GL_POSITION, Light_Position3);
		glLightfv(GL_LIGHT2, GL_AMBIENT, Light_Ambient);
		glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1);
		glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 1);
		glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 1);
		glEnable(GL_LIGHT2);
	glPopMatrix();

	generateShadowMatrix(Light_Position3, verts2);

	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4f(0.1f, 0.1f, 0.1f, 0.5f); // Shadow's colour
	glPushMatrix();
	glMultMatrixf((GLfloat *)shadowMatrix);

	//translate to floor and draw shadow, remember to match any transforms on the object
	glTranslatef(0, 0.0f, 5.5);
	shadowPass = true;
	renderObjects();	//when shadow pass is true, only the cards are rendered
	shadowPass = false;
	glPopMatrix();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // reset colour
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	// Render text, should be last object rendered.
	renderTextOutput();
	
	// Swap buffers, after all objects are rendered.
	glutSwapBuffers();
}

void Scene::renderObjects()
{
	if (shadowPass == false)
	{
		//floor
		glPushMatrix();
			glTranslatef(0, -1, 0);
			glBindTexture(GL_TEXTURE_2D, checkerTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			s.floor();
		glPopMatrix();

		//table
		glPushMatrix();
			glTranslatef(0, -0.75, 0);
			glScalef(0.1, 0.1, 0.1);
			table.render();
		glPopMatrix();

		//teapot and plate
		//marix stack so plates rotate around teapots
		glPushMatrix();
			glScalef(0.1, 0.1, 0.1);
			glPushMatrix();
				glTranslatef(-9, -4.2, 0);
				teapot.render();
				glRotatef(plateRotation, 0, 1, 0);
				glPushMatrix();
					glTranslatef(-3, -1, 0);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(-plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(3, -1, 0);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(-plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0, -1, -3);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(-plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0, -1, 3);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(-plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(9, -4.2, 0);
				teapot.render();
				glRotatef(-plateRotation, 0, 1, 0);
				glPushMatrix();
					glTranslatef(-3, -1, 0);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(3, -1, 0);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0, -1, -3);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
				glPushMatrix();
					glTranslatef(0, -1, 3);
					plate.render();
					glTranslatef(0, 0.2, 0);
					glRotatef(plateRotation * 2, 0, 1, 0);
					cup.render();
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
	}

	//cards
	//cards spawn with rotation speed, height and distance from centre
	spawnCard(1.5, 0, 2.5);
	spawnCard(1.8, 0.1, 2.3);
	spawnCard(-1.2, 0.2, 2.7);
	spawnCard(-1.5, 0.2, 2.8);
	spawnCard(2.0, 0.3, 2.2);
	spawnCard(-1.8, 0.4, 2.5);
	spawnCard(0.9, 0.5, 2.3);
	spawnCard(-1.3, 0.6, 2.6);
	spawnCard(1.3, 0.7, 2.8);
	spawnCard(0.9, 0.8, 2.1);
	spawnCard(-1.7, 0.9, 2.2);
	spawnCard(1.6, 1.0, 2.7);
	spawnCard(-1.8, 1.1, 2.2);

	if (shadowPass == false)
	{
		//ice swan
		glPushMatrix();
			glTranslatef(0, -0.55, 0);
			glScalef(0.1, 0.1, 0.1);
			glEnable(GL_BLEND);
			swan.render();
			glDisable(GL_BLEND);
		glPopMatrix();

		//"Drink Me" bottle
		glPushMatrix();
			glTranslatef(1.5, -0.48, 0.0);
			glScalef(0.1, 0.1, 0.1);
			glDisable(GL_LIGHTING);
			bottlePt2.render();
			glEnable(GL_BLEND);
			bottle.render();
			glEnable(GL_LIGHTING);
			glDisable(GL_BLEND);
		glPopMatrix();
	}
}

// Generates a shadow matrix.
// The shadow matrix will transform an object into a 2D shadow of itself, based on the provide light position and floor plane.
// shadowMatrix[16]; must be declared
// Inputs: light_pos is the light position. floor is a vertex array with 4 vertices that draw a quad (defining the floor plane that the shadow will be drawn on)
void Scene::generateShadowMatrix(float light_pos[4], GLfloat floor[12]) {

	//Defining vertices of plane are PQR with edges PQ and PR
	Vector3 P(floor[0], floor[1], floor[2]);	//top left
	Vector3 Q(floor[3], floor[4], floor[5]);	// bottom left
	Vector3 R(floor[9], floor[10], floor[11]);	// top right

	Vector3 PQ = (Q - P).normalised();
	Vector3 PR = (R - P).normalised();
	Vector3 normal = PR.cross(PQ);

	//Equation of plane is ax + by + cz = d
	//a, b and c are the coefficients of the normal to the plane (i.e. normal = ai + bj + ck)
	//If (x0, y0, z0) is any point on the plane, d = a*x0 + b*y0 + c*z0
	//i.e. d is the dot product of any point on the plane (using P here) and the normal to the plane
	float a, b, c, d;
	a = normal.getX();
	b = normal.getY();
	c = normal.getZ();
	d = normal.dot(P);

	//Origin of projection is at x, y, z. Projection here originating from the light source's position
	float x, y, z;
	x = light_pos[0];
	y = light_pos[1];
	z = light_pos[2];

	//This is the general perspective transformation matrix from a point (x, y, z) onto the plane ax + by + cz = d
	shadowMatrix[0] = d - (b * y + c * z);
	shadowMatrix[1] = a * y;
	shadowMatrix[2] = a * z;
	shadowMatrix[3] = a;

	shadowMatrix[4] = b * x;
	shadowMatrix[5] = d - (a * x + c * z);
	shadowMatrix[6] = b * z;
	shadowMatrix[7] = b;

	shadowMatrix[8] = c * x;
	shadowMatrix[9] = c * y;
	shadowMatrix[10] = d - (a * x + b * y);
	shadowMatrix[11] = c;

	shadowMatrix[12] = -d * x;
	shadowMatrix[13] = -d * y;
	shadowMatrix[14] = -d * z;
	shadowMatrix[15] = -(a * x + b * y + c * z);
}

void Scene::spawnCard(float rotateSpeed, float height, float distance)
{

	glPushMatrix();
		glRotatef(cardRotation * rotateSpeed, 0, 1, 0);		//rotation around origin
		glTranslatef(0, height, distance);

		glPushMatrix();
			glEnable(GL_BLEND);
			glBindTexture(GL_TEXTURE_2D, cardTex);

			glRotatef(cardRotation * 5, 1.0f, 0.8f, 0.6f);	//rotation around itself
			glScalef(0.1, 0.1, 0.1);

			glBegin(GL_QUADS);
				glTexCoord2f(0, 0);
				glNormal3f(0, 1, 0);
				glVertex3f(-1.0f, 0.0f, -1.0f);

				glTexCoord2f(0, 1);
				glNormal3f(0, 1, 0);
				glVertex3f(-1.0f, 0.0f, 1.0f);

				glTexCoord2f(1, 1);
				glNormal3f(0, 1, 0);
				glVertex3f(1.0f, 0.0f, 1.0f);

				glTexCoord2f(1, 0);
				glNormal3f(0, 1, 0);
				glVertex3f(1.0f, 0.0f, -1.0f);
			glEnd();
			glDisable(GL_BLEND);
		glPopMatrix();
	glPopMatrix();

}

void Scene::resize(int w, int h) 
{
	width = w;
	height = h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;
	fov = 45.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(fov, ratio, nearPlane, farPlane);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void Scene::renderTextOutput()
{
	// Render current mouse position and frames per second.
	sprintf_s(mouseText, "Mouse: %i, %i", input->getMouseX(), input->getMouseY());
	displayText(-1.f, 0.96f, 1.f, 0.f, 0.f, mouseText);
	displayText(-1.f, 0.90f, 1.f, 0.f, 0.f, fps);
}

void Scene::displayText(float x, float y, float r, float g, float b, char* string) {
	// Get Lenth of string
	int j = strlen(string);

	// Swap to 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	// Render text.
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, ((float)width/(float)height), nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}
