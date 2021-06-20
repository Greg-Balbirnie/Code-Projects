// Light shader.h
// Basic single light shader setup
#ifndef _TESSELLATIONSHADER_H_
#define _TESSELLATIONSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{
private:
	struct TessFacBufferType
	{
		XMFLOAT4 tessFactors;
	};

public:

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, XMFLOAT4 tessFactors);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessFacBuffer;
	ID3D11SamplerState* sampleState;

};

#endif