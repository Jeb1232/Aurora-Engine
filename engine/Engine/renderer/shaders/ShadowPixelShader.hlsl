struct Input {
	float4 position : SV_POSITION;
	float4 depthPosition : DPOS;
	float2 uv : UV;
};

Texture2D albedoT : register(t0);
SamplerState Sampler : register(s0);

float4 main(Input input) : SV_TARGET
{
	float depthValue;
	float4 albedo = albedoT.Sample(Sampler, input.uv);

	if (albedo.a > 0.5f) {
		depthValue = input.depthPosition / input.depthPosition.w;
	}
	else {
		discard;
	}

	return float4(depthValue, depthValue, depthValue, 1.0f);
}