struct Input {
	float4 position : SV_POSITION;
	float3 camPos : CPOS;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
};

float4 main(Input input) : SV_TARGET
{
	return float4(1.0f,1.0f,1.0f,1.0f);
}