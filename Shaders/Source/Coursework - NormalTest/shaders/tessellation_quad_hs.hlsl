// Tessellation Hull Shader
// Prepares control points for tessellation

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

//tesselation factor buffer
cbuffer CameraBuffer : register(cb0)
{
	float3 cameraPos;
	float padding;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	float3 edge_pos[4];
	float3 centre_position;
	float4 textureColour;	//height map colour

	// Find the edge positions
	edge_pos[0] = (inputPatch[0].position + inputPatch[1].position) / 2;
	edge_pos[1] = (inputPatch[1].position + inputPatch[3].position) / 2;
	edge_pos[2] = (inputPatch[2].position + inputPatch[3].position) / 2;
	edge_pos[3] = (inputPatch[0].position + inputPatch[2].position) / 2;

	// Find the cenre position
	centre_position = (edge_pos[1] + edge_pos[3]) / 2;
	
	// Sample texture colour between vertex 0 and 1
	textureColour = shaderTexture.SampleLevel(SampleType, (inputPatch[0].tex + inputPatch[1].tex) / 2, 0);
	edge_pos[0].y += textureColour.y * 10;

	// Sample texture colour between vertex 1 and 3
	textureColour = shaderTexture.SampleLevel(SampleType, (inputPatch[1].tex + inputPatch[3].tex) / 2, 0);
	edge_pos[1].y += textureColour.y * 10;

	// Sample texture colour between vertex 2 and 3
	textureColour = shaderTexture.SampleLevel(SampleType, (inputPatch[2].tex + inputPatch[3].tex) / 2, 0);
	edge_pos[2].y += textureColour.y * 10;

	// Sample texture colour between vertex 0 and 2
	textureColour = shaderTexture.SampleLevel(SampleType, (inputPatch[0].tex + inputPatch[2].tex) / 2, 0);
	edge_pos[3].y += textureColour.y * 10;

	// Sample texture colour between vertex 1 and 2
	textureColour = shaderTexture.SampleLevel(SampleType, (inputPatch[1].tex + inputPatch[2].tex) / 2, 0);
	centre_position.y += textureColour.y * 10;
	
	// Set the tessellation factors for quad
	output.edges[0] = 2.0f;
	output.edges[1] = 2.0f;
	output.edges[2] = 2.0f;
	output.edges[3] = 2.0f;

	output.inside[0] = 2.0f;
	output.inside[1] = 2.0f;

	// Set the tessellation factors for quad based on distance
	if (distance(cameraPos, edge_pos[0]) < 10.0f)
	{
		output.edges[0] = 12.0f;
	}
	if (distance(cameraPos, edge_pos[1]) < 10.0f)
	{
		output.edges[1] = 12.0f;
	}
	if (distance(cameraPos, edge_pos[2]) < 10.0f)
	{
		output.edges[2] = 12.0f;
	}
	if (distance(cameraPos, edge_pos[3]) < 10.0f)
	{
		output.edges[3] = 12.0f;
	}
	if (distance(cameraPos, centre_position) < 10.0f)
	{
		output.inside[0] = 12.0f;
		output.inside[1] = 12.0f;
	}

	return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;
	
	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;
	
	// Pass the texture throught to the domain shader
	output.tex = patch[pointId].tex;

	// Pass the normal to the domain shader
	output.normal = patch[pointId].normal;

	// Set the input colour as the output colour.
	output.colour = patch[pointId].colour;
	
	return output;
}