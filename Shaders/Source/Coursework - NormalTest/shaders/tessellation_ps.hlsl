// Tessellation pixel shader
// Output colour passed to stage.

Texture2D heightTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D shaderTexture : register(t2);
SamplerState SampleType : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 position3D : TEXCOORD1;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColour;
	float4 diffuseColour;
	float3 lightPosition;
	float padding;
	float3 lightDirection;
	float specularPower;
	float4 specularColour;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 colour;
	float spec;
	float4 finalSpec;
	float3 halfway;
	float4 normal_val;

	// Attenuation variables
	float attenuation;
	float constFac = 1.0f;
	float linFac = 0.125f;
	float quadFac = 0.02f;
	float dist;
	float range = 50.0f;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColour = shaderTexture.Sample(SampleType, input.tex);

	// Sample the normal from the normal map
	normal_val = normalTexture.Sample(SampleType, input.tex);

	// Change normals to be -1 to 1 for normal map
	normal_val = (normal_val * 2) - 1;

	// Set normal to normal value
	input.normal = (normal_val.x * input.tangent) + (normal_val.y * input.normal) + (normal_val.z * input.binormal);	

	// Set ambient
	colour = ambientColour;

	float3 lightDir;
	float lightIntensity;

	// Check what kind of light it is
	if (lightDirection.x == 0.0f && lightDirection.y == 0.0f && lightDirection.z == 0.0f)	// Point light
	{
		// Invert the light direction for calculations.
		lightDir = -(input.position3D - lightPosition);
	}
	else
	{
		lightDir = -(lightDirection);
	}

	// Normalize light vector
	lightDir = normalize(lightDir);

	// Calculate the amount of light on this pixel.
	lightIntensity = max(saturate(dot(input.normal, lightDir)), 0.0);

	// Determine the final amount of diffuse colour based on the diffuse colour combined with the light intensity.
	if (lightIntensity > 0.0f)
	{
		// Calculate diffuse componant
		colour += (diffuseColour * lightIntensity);
		colour = saturate(colour);
	}

	// Add teture to colour
	colour *= textureColour;

	return colour;
}