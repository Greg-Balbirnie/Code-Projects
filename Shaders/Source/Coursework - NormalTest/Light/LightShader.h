// Light shader.h
// Basic single light shader setup
#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"
#include "../DXFramework/Camera.h"

using namespace std;
using namespace DirectX;


class LightShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambientColour[2];
		XMFLOAT4 diffuseColour[2];
		XMFLOAT4 lightPosition[2];
		XMFLOAT4 lightDirection[2];
		XMFLOAT4 lightType[2];
		XMFLOAT4 specularPower[2];
		XMFLOAT4 specularColour[2];
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

public:

	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light, Light* light2, Camera* camera);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
};

#endif