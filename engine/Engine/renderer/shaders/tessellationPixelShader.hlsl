struct Input {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv: UV;
};

Texture2D diffuseT;
SamplerState dSampler;

float4 main(Input input) : SV_TARGET
{
	return diffuseT.Sample(dSampler, input.uv);;
}