
struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
	PSInput s;

	s.position = position;
	s.color = color;

	return s;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}
