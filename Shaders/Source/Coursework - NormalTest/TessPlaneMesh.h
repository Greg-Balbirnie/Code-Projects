// PlaneMesh.h
// Generates a plane mesh based on resolution.
// Resolution is the width and depth of the generated plane, built from unit quads.

#ifndef _TESSPLANEMESH_H_
#define _TESSPLANEMESH_H_

#include "../DXFramework/BaseMesh.h"

using namespace DirectX;

class TessPlaneMesh : public BaseMesh
{

public:
	TessPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 100);
	~TessPlaneMesh();

	void sendData(ID3D11DeviceContext*);

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};

#endif