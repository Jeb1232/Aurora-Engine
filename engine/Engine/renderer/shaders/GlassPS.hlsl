struct Input {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	float2 renUV : RUV;
};

Texture2D renderView : register(t15);
Texture2D normalMap : register(t3);
SamplerState rSampler : register(s14);
SamplerState texSampler : register(s0);

float4 main(Input input) : SV_TARGET
{

	float2 screenUV = input.renUV;
	screenUV.x = (screenUV.x / input.position.w + 1) * 0.5f;
	screenUV.y = 1 - ((screenUV.y / input.position.w + 1) * 0.5f);

	float4 normTex = normalMap.Sample(texSampler, input.uv);
	normTex = normTex * 2.0f - 1.0f;


	float2 offset = normTex * 0.08f;

	float4 renderTex = renderView.Sample(rSampler, screenUV + offset);

	return renderTex;
}