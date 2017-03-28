struct PSInput
{
	float4 position: SV_POSITION;
	float4 color: COLOR;
};

PSInput VSMain(float4 position: POSITION, float4 color : COLOR)
{
	PSInput ps;
	ps.position = position;
	ps.color = color;
	return ps;
}

float4 PSMain(PSInput input):SV_TARGET
{
	float4 color = input.color;
	//color = { 0.0f,0.0f,0.0f,0.0f };
	return color;
}