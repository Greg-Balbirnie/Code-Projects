// Light shader.h
// Basic single light shader setup
#ifndef _DEPTHSHADER_H_
#define _DEPTHSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Camera.h"
#include "../DXFramework/Light.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{
private:
	struct CameraBufferType
	{
		XMFLOAT3 cameraPos;
		float padding;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColour;
		XMFLOAT4 diffuseColour;
		XMFLOAT3 lightPosition;
		float padding;
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 specularColour;
	};

public:

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* height_texture, ID3D11ShaderResourceView* normal_texture, ID3D11ShaderResourceView* texture, Camera* camera, Light* light);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename/*, WCHAR* gsFilename*/, WCHAR* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* sampleState;
};

#endif