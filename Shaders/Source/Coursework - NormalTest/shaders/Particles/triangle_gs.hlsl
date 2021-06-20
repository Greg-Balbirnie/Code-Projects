// Example geometry shader
// Receives a point and outputs a triangle

cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraPosBuffer : register(cb1)
{
	float3 cameraPos;
	float padding;
};

cbuffer PositionBuffer
{
	
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

// Pixel input type
struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

// gs function
[maxvertexcount(4)]

void main(point InputType input[1], inout TriangleStream<OutputType> triStream)
{
	OutputType output;

	float3 g_positions[4] =
	{
		float3(-1, 1, 0) + input[0].position,
		float3(-1, -1, 0) + input[0].position,
		float3(1, 1, 0) + input[0].position,
		float3(1, -1, 0) + input[0].position
	};

	// Set the textures
	float2 g_texes[4] =
	{
		float2(0, 1),
		float2(1, 1),
		float2(0, 0),
		float2(1, 0)
	};

	// Find forward, up and right vectors
	float3 forwardVec;
	float3 rightVec;
	float3 upVec;



	// Find direction of forward
	forwardVec = input[0].position.xyz - cameraPos;
	forwardVec = normalize(forwardVec);

	// Find right vector
	rightVec = cross(forwardVec, float3(0.0f, 1.0f, 0.0f));
	rightVec = normalize(rightVec);
	rightVec /= 10;

	// Find up vector
	upVec = cross(forwardVec, rightVec);
	upVec = normalize(upVec);
	upVec /= 10;

	g_positions[0] = (input[0].position.xyz + upVec + rightVec);
	g_positions[1] = (input[0].position.xyz + upVec - rightVec);
	g_positions[2] = (input[0].position.xyz - upVec + rightVec);
	g_positions[3] = (input[0].position.xyz - upVec - rightVec);

	for (int i = 0; i < 4; i++)
	{
		// Find the position
		float3 vposition = g_positions[i]; 
		output.position = mul(float4(vposition, 1.0), worldMatrix);
		output.position = mul(output.position, viewMatrix);
		output.position = mul(output.position, projectionMatrix);
		
		// Find the texture
		output.tex = g_texes[i];

		output.normal = input[0].normal;
		triStream.Append(output);
	}

	triStream.RestartStrip();
}