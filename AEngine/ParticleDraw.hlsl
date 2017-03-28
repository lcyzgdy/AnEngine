
struct VSParticleIn
{
	float4 color : COLOR;
	uint id : SV_VERTEXID;
};

struct VSParticleDrawOut
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct GSParticleDrawOut
{
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
	float4 pos : SV_POSITION;
};

struct PSParticleDrawIn
{
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

struct PosVelo
{
	float4 pos;
	float4 velo;
};

StructuredBuffer<PosVelo> g_bufPosVelo;

cbuffer cb0
{
	row_major float4x4 g_mWorldViewProj;
	row_major float4x4 g_mInvView;
};

cbuffer cb1
{
	static float g_fParticleRad = 10.0f;
};

cbuffer cbImmutable
{
	static float3 g_positions[4] =
	{
		float3(-1, 1, 0),
		float3(1, 1, 0),
		float3(-1, -1, 0),
		float3(1, -1, 0),
	};
	
	static float2 g_texcoords[4] =
	{ 
		float2(0, 0), 
		float2(1, 0),
		float2(0, 1),
		float2(1, 1),
	};
};

VSParticleDrawOut VSParticleDraw(VSParticleIn input)
{
	VSParticleDrawOut output;
	
	output.pos = g_bufPosVelo[input.id].pos.xyz;
	
	float mag = g_bufPosVelo[input.id].velo.w / 9;
	output.color = lerp(input.color, input.color, mag);

	return output;
}
// 顶点着色器用于绘制点粒子。

[maxvertexcount(4)]
void GSParticleDraw(point VSParticleDrawOut input[1], inout TriangleStream<GSParticleDrawOut> SpriteStream)
{
	GSParticleDrawOut output;
	
	for (int i = 0; i < 4; i++)
	{
		float3 position = g_positions[i] * g_fParticleRad;
		position = mul(position, (float3x3)g_mInvView) + input[0].pos;
		output.pos = mul(float4(position,1.0), g_mWorldViewProj);

		output.color = input[0].color;
		output.tex = g_texcoords[i];
		SpriteStream.Append(output);
	}	// 创建两个新三角形
	SpriteStream.RestartStrip();
}
// GS用于渲染点粒子。取一个点，并将其变成2个三角形（矩形）。

float4 PSParticleDraw(PSParticleDrawIn input) : SV_Target
{
	float intensity = 0.5f - length(float2(0.5f, 0.5f) - input.tex);
	intensity = clamp(intensity, 0.0f, 0.5f) * 2.0f;
	return float4(input.color.xyz, intensity);
}
// PS绘制粒子。 使用纹理坐标生成表示粒子的径向渐变
