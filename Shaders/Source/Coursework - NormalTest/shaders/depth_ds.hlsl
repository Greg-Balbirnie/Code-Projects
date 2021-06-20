// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 position3D : TEXCOORD1;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float4 textureColour;	// Height map colour
	float2 textureLerp;
	float3 vertexPosition;
	float3 normals;
	OutputType output;

	// Calculate height map
	float2 t1 = lerp(patch[0].tex, patch[1].tex, 1 - uvwCoord.y);
	float2 t2 = lerp(patch[2].tex, patch[3].tex, 1 - uvwCoord.y);
	textureLerp = lerp(t1, t2, uvwCoord.x);

	textureColour = shaderTexture.SampleLevel(SampleType, textureLerp, 0);

	float3 v1 = lerp(patch[0].position, patch[1].position, 1 - uvwCoord.y);
	float3 v2 = lerp(patch[2].position, patch[3].position, 1 - uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);
	
	// Offset y based on colour
	vertexPosition.y += textureColour.y * 10;

	// Find normals
	float3 n1 = lerp(patch[0].normal, patch[1].normal, 1 - uvwCoord.y);
	float3 n2 = lerp(patch[2].normal, patch[3].normal, 1 - uvwCoord.y);
	normals = lerp(n1, n2, uvwCoord.x);

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	
	// Send texture to pixel shader
	output.tex = textureLerp;

	// Send normals to pixel shader
	output.normal = normals;

	// Calculate the position of the vertex in the world.
	output.position3D = output.position;
	
	return output;
}

