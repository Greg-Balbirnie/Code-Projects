// SinglePointMesh.h
// A simple mesh for demostrating the geometry shader
// Instead producing a triangle list, produce a point list.
// This point list is three points of a triangle and can be used by the geometry shader to generate geometry.
// Therefore has a modified sendData() function.

#ifndef _SINGLEPOINTMESH_H_
#define _SINGLEPOINTMESH_H_

#include "../DXFramework/BaseMesh.h"
#include <random>

using namespace DirectX;

class SinglePointMesh : public BaseMesh
{
private:
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

public:
	SinglePointMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~SinglePointMesh();

	void sendData(ID3D11DeviceContext*);
	void ChangePosition(ID3D11Device*);

protected:
	void initBuffers(ID3D11Device* device);
	int rando;
};

#endif