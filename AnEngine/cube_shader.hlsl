

cbuffer MatrixBuffer : register (b0) 
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType 
{
	float4 position     : POSITION;
	float4 color        : COLOR;
};

struct PixelInputType 
{
	float4 position     : SV_POSITION;
	float4 color        : COLOR;
};

PixelInputType ColorVertexShader(VertexInputType input) 
{
	PixelInputType output;

	output.position = mul(float4(input.position.xyz, 1), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;

	return output;
}

float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	return input.color;
}