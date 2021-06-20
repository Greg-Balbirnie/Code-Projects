struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 position3D : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
	float depthValue;
	float4 color;

	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue = input.position3D.z / input.position3D.w;
	
	// Output the depth value to texture
	color = float4(depthValue, depthValue, depthValue, 1.0f);
	return color;
}