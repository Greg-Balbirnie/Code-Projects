// Tessellation Hull Shader
// Prepares control points for tessellation

// Tesselation factor buffer
cbuffer TessFacBuffer : register(cb0)
{
	float4 tessFactors;
};

struct InputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;

    // Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tessFactors.x;
	output.edges[1] = tessFactors.y;
	output.edges[2] = tessFactors.z;

    // Set the tessellation factor for tessallating inside the triangle.
	output.inside = tessFactors.w;

    return output;
}


[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input color as the output color.
    output.colour = patch[pointId].colour;

    return output;
}