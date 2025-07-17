#include"Lighting.hlsli"

struct Input {
	float4 position : SV_POSITION;
	float3 camPos : CPOS;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float3 worldPos : WPOS;
	float2 uv: UV;
};

float4 main(Input input) : SV_TARGET
{
	//return float4(PBRdir(float3(1.0f,1.0f,1.0f), input.normal, 0.0f, 0.0f, 0.0f, float3(0.0f,-1.0f,0.5f), input.camPos, input.worldPos, 0.0f, float3(1.0f,1.0f,1.0f), 10.0f), 1.0f);
	return float4(1.0f,1.0f,1.0f,1.0f);
}