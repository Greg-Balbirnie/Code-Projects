Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 position3D : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 colour;

	// Sample the texture
	textureColour = texture0.Sample(Sampler0, input.tex);

	//clip for alpha
	clip(textureColour.a - 0.5);

	// Multiply the texture pixel and the final diffuse colour to get the final pixel colour result.
	colour = textureColour;

	// Clamp colour value
	colour = saturate(colour);

	return colour;
}