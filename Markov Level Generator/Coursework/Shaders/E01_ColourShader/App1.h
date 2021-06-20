// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "../DXFramework/DXF.h"
#include "ColourShader.h"
#include "CustomPlaneMesh.h"
#include "TextureShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "ratio.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);

	bool frame();

	void RenderToTexture();
	void DownSample();
	void HorizontalBlur();
	void VerticalBlur();
	void UpSample();
	void RenderScene();

protected:
	bool render();
	void gui();

private:
	// Shaders
	ColourShader* colourShader;
	TextureShader* texture_shader;
	HorizontalBlurShader* hBlurShader;
	VerticalBlurShader* vBlurShader;

	// Meshes
	QuadMesh* mesh;
	CustomPlaneMesh* plane_mesh;
	
	// Markov chain level generator
	//Improved_Markov* level_generator;

	// Ratio finder
	Ratio ratio_finder;

	// Render textures
	RenderTexture* sceneTexture;
	RenderTexture* downTexture;
	RenderTexture* hBlurTexture;
	RenderTexture* vBlurTexture;
	RenderTexture* upTexture;

	// Ortho meshes
	OrthoMesh* downOrthoMesh;
	OrthoMesh* upOrthoMesh;

	// Screen size
	int width, height;

	// GUI variables
	bool is_blur;
};

#endif