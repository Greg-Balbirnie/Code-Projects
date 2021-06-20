// plane mesh
// Quad mesh made of many quads. Default is 100x100
#include "CustomPlaneMesh.h"
#include <vector>

// Initialise buffer and load texture.
CustomPlaneMesh::CustomPlaneMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution)
{
	resolution = lresolution;
	initBuffers(device);
}

// Release resources.
CustomPlaneMesh::~CustomPlaneMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Generate plane (including texture coordinates and normals).
void CustomPlaneMesh::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	float positionX, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	
	// Calculate the number of vertices in the terrain mesh.
	vertexCount = (resolution - 1) * (resolution - 1) * 8;


	indexCount = vertexCount;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];
	

	index = 0;
	// UV coords.
	u = 0;
	v = 0;
	increment = 1.0f / resolution;
	
	int rand_val = 5;

	// Create the flat plane
	for (j = 0; j<(resolution - 1); j++)
	{
		for (i = 0; i<(resolution - 1); i++)
		{
			// Bottom left.
			positionX = (float)i;
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, rand() % rand_val, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, rand() % rand_val, positionZ);
			vertices[index].texture = XMFLOAT2(u + increment, v + increment);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;


			// Upper left
			positionX = (float)(i);
			positionZ = (float)(j + 1);
			

			vertices[index].position = XMFLOAT3(positionX, rand() % rand_val, positionZ);
			vertices[index].texture = XMFLOAT2(u, v + increment);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Bottom left
			positionX = (float)(i);
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, rand() % rand_val, positionZ);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Bottom right
			positionX = (float)(i + 1);
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, rand() % rand_val, positionZ);
			vertices[index].texture = XMFLOAT2(u + increment, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Upper right.
			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, rand() % rand_val, positionZ);
			vertices[index].texture = XMFLOAT2(u + increment, v + increment);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			u += increment;
	
		}

		u = 0;
		v += increment;
	}

	float multiplier = 100;

	// Add noise
	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(vertices[i].position.x, noise_generator.create_noise(vertices[i].position.x + 0.2, 0.0f, vertices[i].position.z + 0.2) * multiplier, vertices[i].position.z);
	}

	float is_smooth = true;

	if (is_smooth)
	{
		for (int x = 0; x < 5; x++)
		{
			std::vector<float> heights;

			// Reorder list to smooth
			for (i = 0; i < (resolution - 1); i++)
			{
				int k = (i * 6);
			
				// The top row is just above this row
				// Add top left to heights (+2 for top left)
				heights.push_back(vertices[k + 2].position.y);
				// Check if at the end of the row 
				if (i == (resolution - 2))
				{
					// Add top right to heights (+1 for top right)
					heights.push_back(vertices[k + 1].position.y);
				}
			}

			for (j = 0; j < (resolution - 1); j++)
			{
				for (i = 0; i < (resolution - 1); i++)
				{
					int k = (j * resolution * 6) + (i * 6);

					// Add bottom left to heights (+0 for bottom left)
					heights.push_back(vertices[k].position.y);
					// Check if at the end of the row 
					if (i == (resolution - 2))
					{
						// Add bottom right to heights (+4 for top right)
						heights.push_back(vertices[k + 4].position.y);
					}
				}
			}

			int counter = 1;

			// Smooth
			for (int i = 0; i < heights.size(); i++)
			{
				// Set counter to 1
				counter = 1;

				// Check top left is in plane
				if ((i - (resolution + 1) > 0))
				{
					// Add height to vertex
					heights[i] += heights[i - (resolution + 1)];
					counter++;
				}
				// Check top is in plane
				if ((i - (resolution) > 0))
				{
					// Add height to vertex
					heights[i] += heights[i - (resolution)];
					counter++;
				}
				// Check top right is in plane
				if ((i - (resolution - 1) > 0))
				{
					// Add height to vertex
					heights[i] += heights[i - (resolution - 1)];
					counter++;
				}
				// Check left is in plane
				if ((i - 1) > 0)
				{
					// Add height to vertex
					heights[i] += heights[i - 1];
					counter++;
				}

				// Check right is in plane
				if ((i + 1) < ((resolution * resolution)))
				{
					// Add height to vertex
					heights[i] += heights[i + 1];
					counter++;
				}
				// Check bottom left is in plane
				if ((i + (resolution - 1)) < ((resolution * resolution)))
				{
					// Add height to vertex
					heights[i] += heights[i + (resolution - 1)];
					counter++;
				}
				// Check bottom is in plane
				if ((i + (resolution)) < ((resolution * resolution)))
				{
					// Add height to vertex
					heights[i] += heights[i + resolution];
					counter++;
				}
				// Check bottom right is in plane
				if ((i + (resolution + 1)) < ((resolution * resolution)))
				{
					// Add height to vertex
					heights[i] += heights[i + (resolution + 1)];
					counter++;
				}
				// Divide by counter to get average
				heights[i] /= (float)counter;
			}

			// Reset index
			index = 0;
			float k = 0;

			// Set to smoother heights
			for (j = 0; j < (resolution - 1); j++)
			{
				for (i = 0; i < (resolution - 1); i++)
				{
					k = (resolution * j) + i;
					vertices[index].position = XMFLOAT3(vertices[index].position.x, heights[k], vertices[index].position.z);
					index++;

					k = (resolution * (j + 1)) + (i + 1);
					vertices[index].position = XMFLOAT3(vertices[index].position.x, heights[k], vertices[index].position.z);
					index++;

					k = (resolution * (j + 1)) + i;
					vertices[index].position = XMFLOAT3(vertices[index].position.x, heights[k], vertices[index].position.z);
					index++;

					k = (resolution * j) + i;
					vertices[index].position = XMFLOAT3(vertices[index].position.x, heights[k], vertices[index].position.z);
					index++;

					k = (resolution * j) + (i + 1);
					vertices[index].position = XMFLOAT3(vertices[index].position.x, heights[k], vertices[index].position.z);
					index++;

					k = (resolution * (j + 1)) + (i + 1);
					vertices[index].position = XMFLOAT3(vertices[index].position.x, heights[k], vertices[index].position.z);
					index++;
				}
			}
		}
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
	
	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}


