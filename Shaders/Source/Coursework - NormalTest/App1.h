// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "ColourShader.h"
#include "TessellationShader.h"
#include "TessellationQuadShader.h"
#include "TestMesh.h"
#include "TessQuadMesh.h"
#include "TessPlaneMesh.h"
#include "DepthShader.h"
#include "Particles\GeometryShader.h"
#include "Particles\SinglePointMesh.h"
#include "DoF\HorizontalBlurShader.h"
#include "DoF\VerticalBlurShader.h"
#include "DoF\TextureShader.h"
#include "DepthBlurShader.h"
#include "Light\LightShader.h"
#include "Shadow\ShadowShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);

	bool frame();

protected:
	bool render();
	void RenderDepth();
	void RenderShadowDepth();
	void RenderToTexture();
	void DownSample();
	void HorizontalBlur();
	void VerticalBlur();
	void UpSample();
	void RenderScene();
	void gui();

private:
	// Shaders
	ColourShader* colourShader;
	TessellationShader* tesselationShader;
	TessellationQuadShader* tesselationQuadShader;
	DepthShader* depthShader;
	GeometryShader* geometryShader;
	DepthBlurShader* depthBlurShader;
	TextureShader* textureShader;
	HorizontalBlurShader* hBlurShader;
	VerticalBlurShader* vBlurShader;
	LightShader* lightShader;
	ShadowShader* shadowShader;

	// Meshes
	TestMesh* mesh;
	TessQuadMesh* quadMesh;
	TessPlaneMesh* planeMesh;
	OrthoMesh* orthoMesh;
	SphereMesh* sphereMesh;
	PlaneMesh* shadowPlaneMesh;
	SphereMesh* shadowSphereMesh;

	// Render textures
	RenderTexture* depthTexture;
	RenderTexture* shadowDepthTexture;
	RenderTexture* sceneTexture;
	RenderTexture* downTexture;
	RenderTexture* hBlurTexture;
	RenderTexture* vBlurTexture;
	RenderTexture* upTexture;

	// Meshes
	SinglePointMesh* pointMesh;

	// Gui variables
	bool isWireframe;
	bool isBlur;

	// Lighting
	Light* light;
	Light* light2;
	Light* shadowLight;

	// Screen width and Height
	int width, height;

	// The position of the sphere and plane for shadows
	XMFLOAT3 spherePos;
	XMFLOAT3 planePos;
};

#endif