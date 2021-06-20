// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	//BaseApplication::BaseApplication();
	mesh = nullptr;
	colourShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	plane_mesh = new CustomPlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	mesh = new QuadMesh(renderer->getDevice(), renderer->getDeviceContext());

	// Create texture shader to render level on a plane
	texture_shader = new TextureShader(renderer->getDevice(), hwnd);

	// Create shaders for gaussian blur
	hBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	vBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);

	// generte the level as PNG
	//level_generator->Generate();
	textureMgr->loadTexture("level", L"../generated_level.png");
	textureMgr->loadTexture("bunny", L"../res/bunny.png");

	//create render texture
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	hBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	vBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//create upOrthoMesh
	//downOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2, 0, 0);
	upOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);

	//set screen width and height
	width = screenWidth;
	height = screenHeight;
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

	// Delete shaders
	if (colourShader)
	{
		delete colourShader;
		colourShader = 0;
	}

	if (texture_shader)
	{
		delete texture_shader;
		texture_shader = 0;
	}

	if (hBlurShader)
	{
		delete hBlurShader;
		hBlurShader = 0;
	}

	if (vBlurShader)
	{
		delete vBlurShader;
		vBlurShader = 0;
	}

	// Delete meshes
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (plane_mesh)
	{
		delete plane_mesh;
		plane_mesh = 0;
	}

	/*if (level_generator)
	{
		delete level_generator;
		level_generator = 0;
	}*/
	
	if (sceneTexture)
	{
		delete sceneTexture;
		sceneTexture = 0;
	}

	if (downTexture)
	{
		delete downTexture;
		downTexture = 0;
	}

	if (hBlurTexture)
	{
		delete hBlurTexture;
		hBlurTexture = 0;
	}

	if (vBlurTexture)
	{
		delete vBlurTexture;
		vBlurTexture = 0;
	}

	if (upTexture)
	{
		delete upTexture;
		upTexture = 0;
	}

	// Delete ortho meshes
	if (downOrthoMesh)
	{
		delete downOrthoMesh;
		downOrthoMesh = 0;
	}

	if (upOrthoMesh)
	{
		delete upOrthoMesh;
		upOrthoMesh = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();

	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//// Clear the scene. (The old blue colour is out, I'll find a better one)
	renderer->beginScene(0.0f, 0.38f, 0.22f, 1.0f);

	//// Generate the view matrix based on the camera's position.
	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//render to texture
	RenderToTexture();

	// Only do these to blur the image
	if (is_blur)
	{
		//down sample
		DownSample();

		//horizontal blur
		HorizontalBlur();

		//vertical blur
		VerticalBlur();

		//up sample
		UpSample();
	}

	//render scene
	RenderScene();

	// Render GUI
	gui();

	//// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::RenderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	sceneTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.38f, 0.22f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//// Send geometry data (from plane_mesh)
	plane_mesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("bunny"));
	//// Render object (combination of mesh geometry and shader process
	texture_shader->render(renderer->getDeviceContext(), plane_mesh->getIndexCount());

	worldMatrix = XMMatrixScaling(ratio_finder.Return_Ratio().x, ratio_finder.Return_Ratio().y, 1.0f);

	//// Send geometry data (from plane_mesh)
	mesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("level"));
	//// Render object (combination of mesh geometry and shader process
	texture_shader->render(renderer->getDeviceContext(), plane_mesh->getIndexCount());

	//set render target to back buffer
	renderer->setBackBufferRenderTarget();
}

void App1::DownSample()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//set render target to back buffer
	downTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.0f, 0.38f, 0.22f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// To render ortho mesh
	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// ortho matrix for 2D rendering
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	upOrthoMesh->sendData(renderer->getDeviceContext());
	texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView());
	texture_shader->render(renderer->getDeviceContext(), upOrthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	//set render target to back buffer
	renderer->setBackBufferRenderTarget();
}

void App1::HorizontalBlur()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//set render target to back buffer
	hBlurTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.0f, 0.38f, 0.22f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// To render ortho mesh
	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// ortho matrix for 2D rendering
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	upOrthoMesh->sendData(renderer->getDeviceContext());
	hBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downTexture->getShaderResourceView(), width);
	hBlurShader->render(renderer->getDeviceContext(), upOrthoMesh->getIndexCount());

	//texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView(), textureMgr->getTexture("default"));
	//texture_shader->render(renderer->getDeviceContext(), upOrthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	//set render target to back buffer
	renderer->setBackBufferRenderTarget();
}

void App1::VerticalBlur()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//set render target
	vBlurTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.0f, 0.38f, 0.22f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// To render ortho mesh
	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// ortho matrix for 2D rendering
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	upOrthoMesh->sendData(renderer->getDeviceContext());
	vBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, hBlurTexture->getShaderResourceView(), height);
	vBlurShader->render(renderer->getDeviceContext(), upOrthoMesh->getIndexCount());

	//texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView(), textureMgr->getTexture("default"));
	//texture_shader->render(renderer->getDeviceContext(), upOrthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	//set render target to back buffer
	renderer->setBackBufferRenderTarget();
}

void App1::UpSample()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//set render target to back buffer
	upTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.0f, 0.38f, 0.22f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// To render ortho mesh
	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// ortho matrix for 2D rendering
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	upOrthoMesh->sendData(renderer->getDeviceContext());
	texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, vBlurTexture->getShaderResourceView());
	texture_shader->render(renderer->getDeviceContext(), upOrthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	//set render target to back buffer
	renderer->setBackBufferRenderTarget();
}

void App1::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.0f, 0.38f, 0.22f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// To render ortho mesh
	// Turn off the Z buffer to begin all 2D rendering.
	renderer->setZBuffer(false);

	// ortho matrix for 2D rendering
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	upOrthoMesh->sendData(renderer->getDeviceContext());
	// Use up texture if blurring
	if (is_blur)
	{
		texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, upTexture->getShaderResourceView());
	}
	// use scene texture if not
	else
	{
		texture_shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView());
	}
	texture_shader->render(renderer->getDeviceContext(), upOrthoMesh->getIndexCount());

	renderer->setZBuffer(true);
}

void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Blur", &is_blur);

	// Render UI
	ImGui::Render();
}

