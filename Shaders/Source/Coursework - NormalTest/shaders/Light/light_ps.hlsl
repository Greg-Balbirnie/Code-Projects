// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
	float4 ambientColour[2];
	float4 diffuseColour[2];
	float4 lightPosition[2];
	float4 lightDirection[2];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	float3 position3D : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float4 colour[2];
	float4 finalColour;

	// Attenuation variables
	float attenuation;
	float constFac = 1.0f;
	float linFac = 0.125f;
	float quadFac = 0.02f;
	float dist;
	float range = 50.0f;

    // Sample the pixel colour from the texture using the sampler at this texture coordinate location.
    textureColour = shaderTexture.Sample(SampleType, input.tex);

	// Set ambient
	finalColour = ambientColour[0];

	// For all lights
	for (int i = 0; i < 2; i++)
	{
		float3 lightDir;
		float lightIntensity;

		// Init light
		colour[i] = float4(0.0f, 0.0f, 0.0f, 1.0f);

		// Check what kind of light it is
		if (lightDirection[i].x == 0.0f && lightDirection[i].y == 0.0f && lightDirection[i].z == 0.0f)
		{
			// Invert the light direction for calculations.
			lightDir = -(input.position3D - lightPosition[i]);
		}
		else
		{ 
			lightDir = -(lightDirection[i]);
		}
		 
		// Calculate distance
		dist = length(lightDir);

		if (dist < range)
		{
			// Normalize light vector
			lightDir = normalize(lightDir);

			// Calculate the amount of light on this pixel.
			lightIntensity = max(saturate(dot(input.normal, lightDir)), 0.0);

			// Determine the final amount of diffuse colour based on the diffuse colour combined with the light intensity.
			if (lightIntensity > 0.0f)
			{
				// Calculate diffuse componant
				colour[i] = (diffuseColour[i] * lightIntensity);
				colour[i] = saturate(colour[i]);

				// Calculate attenuation
				attenuation = 1.0f / (constFac + linFac * dist + quadFac * (dist * dist));
				colour[i] *= attenuation;
			}
		}
	}

	// Combine colour
	finalColour += colour[0] + colour[1];
	finalColour = saturate(finalColour);

	// Multiply the texture pixel and the final diffuse colour to get the final pixel colour result.
	finalColour *= textureColour;

	return finalColour;
}