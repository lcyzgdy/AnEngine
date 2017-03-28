cbuffer SceneConstantBuffer : register(b0)
{
	float4 offset;
}

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
	PSInput re;
	re.position = position + offset;
	re.color = color;
	return re;
}

float4 PSMain(PSInput pixel) : SV_TARGET
{
	return pixel.color; 
}