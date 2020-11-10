cbuffer SceneConstantBuffer : register(b0)
{
	float4 velocity;
	float4 offset;
	float4 color;
	float4x4 projection;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION)
{
	PSInput result;

	result.position = mul(position + offset, projection);

	float intensity = saturate((4.0f - result.position.z) / 2.0f);
	result.color = float4(color.xyz * intensity, 1.0f);

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}