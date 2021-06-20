// CustomPlaneMesh.h
// Generates a plane mesh based on resolution.
// Resolution is the width and depth of the generated plane, built from unit quads.

#ifndef _CustomPlaneMesh_H_
#define _CustomPlaneMesh_H_

#include "../DXFramework/BaseMesh.h"
#include "noise.h"

class CustomPlaneMesh : public BaseMesh
{

public:
	CustomPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 100);
	~CustomPlaneMesh();

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
	noise noise_generator;
};

#endif