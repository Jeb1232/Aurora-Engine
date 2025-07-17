struct Input {
	float4 position : SV_POSITION;
	float2 uv : UV;
};

Texture2D renderView : register(t15);
Texture2D depthTexture : register(t14);
SamplerState rSampler : register(s14);

float4 main(Input input) : SV_TARGET
{
	float3 renderTex = renderView.Sample(rSampler, input.uv);
	float depth = depthTexture.Sample(rSampler, input.uv).r;
	float3 col = float3(0.0f, 0.0f, 0.0f);
	if (depth == 1) {
		col = renderTex;
	}

	return float4(col, 1.0f);
}