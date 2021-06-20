// Point mesh.
// For geometry shader demonstration.
// Note sendData() override.
#include "SinglePointMesh.h"

// Initialise buffers and load texture.
SinglePointMesh::SinglePointMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);
}

// Release resources.
SinglePointMesh::~SinglePointMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Generate point mesh. Simple triangle.
void SinglePointMesh::initBuffers(ID3D11Device* device)
{
	vertexCount = 10000;
	indexCount = 10000;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	//loop to create all points
	for (int i = 0; i < vertexCount; i++)
	{
		// Load the vertex array with data.
		vertices[i].position = XMFLOAT3((rand() % 100 - 50) + 50, (rand() % 100), (rand() % 100 - 50) + 50);
		vertices[i].texture = XMFLOAT2(0.0f, 0.0f);
		vertices[i].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// Load the index array with data.
		indices[i] = i;
	} 

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}

// Override sendData()
// Change in primitive topology (pointlist instead of trianglelist) for geometry shader use.
void SinglePointMesh::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

//change the position of the points
void SinglePointMesh::ChangePosition(ID3D11Device* device)
{
	// Loop to create all points
	for (int i = 0; i < vertexCount; i++)
	{
		// Load the vertex array with data.
		vertices[i].position = XMFLOAT3(vertices[i].position.x + (rand() % 3 - 1) * 0.05, vertices[i].position.y -= 0.05f, vertices[i].position.z + (rand() % 3 - 1) * 0.05);
		if (vertices[i].position.y <= 0.0f)
		{
			vertices[i].position.y = 100.0f;
		}
		//vertices[i].position = XMFLOAT3(vertices[i].position.x, vertices[i].position.y, i);
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}


