// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	// Set up meshes
	mesh = nullptr;
	quadMesh = nullptr;
	planeMesh = nullptr;
	pointMesh = nullptr;
	orthoMesh = nullptr;
	sphereMesh = nullptr;
	shadowPlaneMesh = nullptr;
	shadowSphereMesh = nullptr;

	// Set up shaders
	colourShader = nullptr;
	tesselationShader = nullptr;
	tesselationQuadShader = nullptr;
	depthShader = nullptr;
	geometryShader = nullptr;
	depthBlurShader = nullptr;
	vBlurShader = nullptr;
	hBlurShader = nullptr;
	textureShader = nullptr;
	lightShader = nullptr;
	shadowShader = nullptr;

	// Set up textures
	depthTexture = nullptr;
	shadowDepthTexture = nullptr;
	sceneTexture = nullptr;
	downTexture = nullptr;
	hBlurTexture = nullptr;
	vBlurTexture = nullptr;
	upTexture = nullptr;

	// Set up lights
	light = nullptr;
	light2 = nullptr;
	shadowLight = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// Create Mesh object
	mesh = new TestMesh(renderer->getDevice(), renderer->getDeviceContext());
	quadMesh = new TessQuadMesh(renderer->getDevice(), renderer->getDeviceContext());
	planeMesh = new TessPlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	pointMesh = new SinglePointMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	shadowPlaneMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	shadowSphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	// Create shaders
	colourShader = new ColourShader(renderer->getDevice(), hwnd);
	tesselationShader = new TessellationShader(renderer->getDevice(), hwnd);
	tesselationQuadShader = new TessellationQuadShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	geometryShader = new GeometryShader(renderer->getDevice(), hwnd);
	hBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	vBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthBlurShader = new DepthBlurShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Declare shadowmap
	const int SHADOWMAP_WIDTH = 1024;
	const int SHADOWMAP_HEIGHT = 1024;

	// Load heightmap
	textureMgr->loadTexture("height_map", L"../res/height.png");
	// Load normal map
	textureMgr->loadTexture("normal_map", L"../res/NormalMap.png");
	// Load rock texture
	textureMgr->loadTexture("rock", L"../res/rock.png");
	// Load rock texture
	textureMgr->loadTexture("bean_child", L"../res/bean_child.png");
	// Load snowfalke textures
	textureMgr->loadTexture("snowflake", L"../res/snowflake.png");

	// Initialise gui variables
	isWireframe = false;
	isBlur = false;

	// Create light
	light = new Light;
	float ambVal = 0.3f;
	light->setAmbientColour(ambVal, ambVal, ambVal, 1.0f);
	light->setDiffuseColour(0.7f, 0.7f, 0.7f, 1.0f);
	light->setPosition(0.0f, 0.0f, 0.0f);
	light->setDirection(1.0f, -1.0f, 1.0f);
	light->setSpecularPower(0.0f);
	light->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);

	// Create light 2
	light2 = new Light;
	ambVal = 0.0f;
	light2->setAmbientColour(ambVal, ambVal, ambVal, 1.0f);
	light2->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	light2->setPosition(48.0f, 10.0f, 48.0f);
	light2->setDirection(0.0f, 0.0f, 0.0f);
	light2->setSpecularPower(10.0f);
	light2->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);

	// Set up sphere and plane offset values
	spherePos = XMFLOAT3(5.0f, -5.0f, -5.0f);
	planePos = XMFLOAT3(0.0f, -10.0f, -10.0f);

	// Create Shadow Light
	shadowLight = new Light;
	ambVal = 0.0f;
	shadowLight->setAmbientColour(ambVal, ambVal, ambVal, 1.0f);
	shadowLight->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	shadowLight->setPosition(5.0f, 10.0f, -5.0f);
	shadowLight->setLookAt(spherePos.x, spherePos.y, spherePos.z);
	shadowLight->setPosition(50.0f, 20.0f, 50.0f);
	shadowLight->setLookAt(50.0f, 0.0f, 50.0f);
	shadowLight->setSpecularPower(10.0f);
	shadowLight->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	shadowLight->generateViewMatrix();
	shadowLight->generateProjectionMatrix(1.0f, SCREEN_DEPTH);
	
	// Create render texture
	depthTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	shadowDepthTexture = new RenderTexture(renderer->getDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH);
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downTexture = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);
	hBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);
	vBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);
	upTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Create orthoMesh
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);

	// Set screen width and height
	width = screenWidth;
	height = screenHeight;
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = 0;
	}

	if (planeMesh)
	{
		delete planeMesh;
		planeMesh = 0;
	}

	if (pointMesh)
	{
		delete pointMesh;
		pointMesh = 0;
	}

	if (sphereMesh)
	{
		delete sphereMesh;
		sphereMesh = 0;
	}

	if (colourShader)
	{
		delete colourShader;
		colourShader = 0;
	}

	if (tesselationShader)
	{
		delete tesselationShader;
		tesselationShader = 0;
	}

	if (tesselationQuadShader)
	{
		delete tesselationQuadShader;
		tesselationQuadShader = 0;
	}

	if (depthShader)
	{
		delete depthShader;
		depthShader = 0;
	}

	if (geometryShader)
	{
		delete geometryShader;
		geometryShader = 0;
	}

	if (geometryShader)
	{
		delete geometryShader;
		geometryShader = 0;
	}

	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}

	if (shadowShader)
	{
		delete shadowShader;
		shadowShader = 0;
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

	// Change particle position
	pointMesh->ChangePosition(renderer->getDevice());

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
	// Do a depth pass for the blur
	RenderDepth();

	// Do a depth pass for the shadow
	RenderShadowDepth();

	// Render to texture
	RenderToTexture();

	// Check if blur is on
	if (isBlur)
	{
		// Down sample
		DownSample();

		// Horizontal blur
		HorizontalBlur();

		// Vertical blur
		VerticalBlur();

		// Up sample
		UpSample();
	}
	// Render scene
	RenderScene();

	// Render GUI
	gui();

	//// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}

void App1::RenderDepth()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	
	// Set the render target to be the render to texture.
	depthTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	depthTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//// Send geometry data (from quadMesh)
	planeMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height_map"), textureMgr->getTexture("normal_map"), textureMgr->getTexture("rock"), camera, light);
	//// Render object (combination of quadMesh geometry and shader process
	depthShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());
	
	//// Send geometry data (from pointMesh)
	pointMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	depthBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	//// Render object (combination of pointMesh geometry and shader process
	depthBlurShader->render(renderer->getDeviceContext(), pointMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(50.0f, 10.0f, 50.0f);

	//// Send geometry data (from pointMesh)
	sphereMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	depthBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	//// Render object (combination of pointMesh geometry and shader process
	depthBlurShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(spherePos.x, spherePos.y, spherePos.z);

	//// Send geometry data (from pointMesh)
	shadowSphereMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	depthBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	//// Render object (combination of pointMesh geometry and shader process
	depthBlurShader->render(renderer->getDeviceContext(), shadowSphereMesh->getIndexCount());

	worldMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1) * XMMatrixTranslation(planePos.x, planePos.y, planePos.z);

	//// Send geometry data (from pointMesh)
	shadowPlaneMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	depthBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix) ;
	//// Render object (combination of pointMesh geometry and shader process
	depthBlurShader->render(renderer->getDeviceContext(), shadowPlaneMesh->getIndexCount());

	// set the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::RenderShadowDepth()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	shadowDepthTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	shadowDepthTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = shadowLight->getViewMatrix();
	projectionMatrix = shadowLight->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(spherePos.x, spherePos.y, spherePos.z);

	//// Send geometry data (from pointMesh)
	shadowSphereMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	depthBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	//// Render object (combination of pointMesh geometry and shader process
	depthBlurShader->render(renderer->getDeviceContext(), shadowSphereMesh->getIndexCount());

	worldMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1) * XMMatrixTranslation(planePos.x, planePos.y, planePos.z);

	//// Send geometry data (from pointMesh)
	shadowPlaneMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	depthBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
	//// Render object (combination of pointMesh geometry and shader process
	depthBlurShader->render(renderer->getDeviceContext(), shadowPlaneMesh->getIndexCount());

	// set the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::RenderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Set the render target to be the render to texture.
	sceneTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the render to texture.
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, -10.0f);

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Check if wireframe mode is on
	if (isWireframe)
	{
		renderer->setWireframeMode(true);
	}

	//// Send geometry data (from quadMesh)
	planeMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	tesselationQuadShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("height_map"), textureMgr->getTexture("normal_map"), textureMgr->getTexture("rock"), camera, light);
	//// Render object (combination of quadMesh geometry and shader process
	tesselationQuadShader->render(renderer->getDeviceContext(), planeMesh->getIndexCount());

	// Allow alpha blending
	renderer->setAlphaBlending(true);

	//// Send geometry data (from pointMesh)
	pointMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	geometryShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("snowflake"), light, camera);
	//// Render object (combination of pointMesh geometry and shader process
	geometryShader->render(renderer->getDeviceContext(), pointMesh->getIndexCount());

	// Disable alpha blending
	renderer->setAlphaBlending(false);

	worldMatrix = XMMatrixTranslation(50.0f, 10.0f, 50.0f);

	//// Send geometry data (from pointMesh)
	sphereMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("default"), light, light2, camera);
	//// Render object (combination of pointMesh geometry and shader process
	lightShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(spherePos.x, spherePos.y, spherePos.z);

	//// Send geometry data (from pointMesh)
	shadowSphereMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("default"), shadowDepthTexture->getShaderResourceView(), shadowLight);
	//// Render object (combination of pointMesh geometry and shader process
	shadowShader->render(renderer->getDeviceContext(), shadowSphereMesh->getIndexCount());

	worldMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1) * XMMatrixTranslation(planePos.x, planePos.y, planePos.z);

	//// Send geometry data (from pointMesh)
	shadowPlaneMesh->sendData(renderer->getDeviceContext());
	//// Set shader parameters (matrices and texture)
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("default"), shadowDepthTexture->getShaderResourceView(), shadowLight);
	//// Render object (combination of pointMesh geometry and shader process
	shadowShader->render(renderer->getDeviceContext(), shadowPlaneMesh->getIndexCount());


	if (isWireframe)
	{
		renderer->setWireframeMode(false);
	}

	// set the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::DownSample()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	// Set render target to back buffer
	downTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

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

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	//set render target to back buffer
	renderer->setBackBufferRenderTarget();
}

void App1::HorizontalBlur()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	// Set render target to back buffer
	hBlurTexture->setRenderTarget(renderer->getDeviceContext());

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

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

	orthoMesh->sendData(renderer->getDeviceContext());
	hBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downTexture->getShaderResourceView(), depthTexture->getShaderResourceView(), width);
	hBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

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
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

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

	orthoMesh->sendData(renderer->getDeviceContext());
	vBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, hBlurTexture->getShaderResourceView(), depthTexture->getShaderResourceView(), height);
	vBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

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
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

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

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, vBlurTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	//set render target to back buffer
	renderer->setBackBufferRenderTarget();
}

void App1::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

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

	orthoMesh->sendData(renderer->getDeviceContext());

	// If the blur is on
	if (isBlur)
	{
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, upTexture->getShaderResourceView());
	}
	else
	{
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView());
	}
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);
}

void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	// FPS
	ImGui::Text("FPS: %.2f", timer->getFPS());

	// Wireframe mode
	ImGui::Checkbox("Wireframe Mode", &isWireframe);

	// Depth Blur
	ImGui::Checkbox("Depth Blur", &isBlur);

	// Render UI
	ImGui::Render();
}