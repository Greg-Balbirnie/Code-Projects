// Example geometry shader
// Receives a point and outputs a triangle

cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer PositionBuffer
{
	static float3 g_positions[4] =
	{
		float3(-1, 1, 0),
		float3(-1, -1, 0),
		float3(1, 1, 0),
		float3(1, -1, 0)
	};
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 position3D : TEXCOORD2;
};

// pixel input type
struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 position3D : TEXCOORD2;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

// gs function
[maxvertexcount(6)]

void main(triangle InputType input[3], inout TriangleStream<OutputType> triStream)
{
	OutputType output;

	// Find normal of triangle
	float3 vec1 = (input[1].position - input[0].position);
	float3 vec2 = (input[2].position - input[1].position);
	output.normal = cross(vec1, vec2);
	output.normal = normalize(output.normal);

	output.normal = float3(0.0f, 1.0f, 0.0f);
	output.normal = normalize(output.normal);

	// Find binormal from normal 
	output.binormal = cross(output.normal, float3(0.0f, 1.0f, 0.0f));
	output.binormal = normalize(output.binormal);

	// Find tangent from normal and Binormal
	output.tangent = cross(output.normal, output.binormal);
	output.tangent = normalize(output.tangent);

	// Set output position, texture normal ana worldspace position to input values
	output.position = input[0].position;
	output.tex = input[0].tex;
	output.colour = input[0].colour;
	output.position3D = input[0].position3D;

	triStream.Append(output);

	// Set output position, texture normal ana worldspace position to input values
	output.position = input[1].position;
	output.tex = input[1].tex;
	output.colour = input[1].colour;
	output.position3D = input[1].position3D;

	triStream.Append(output);

	// Set output position, texture normal ana worldspace position to input values
	output.position = input[2].position;
	output.tex = input[2].tex;
	output.colour = input[2].colour;
	output.position3D = input[2].position3D;

	triStream.Append(output);

	triStream.RestartStrip();
}