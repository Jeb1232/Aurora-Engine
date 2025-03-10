struct Input {
    float4 position : SV_POSITION;
    float2 uv: UV;
};

Texture2D renderView : register(t15);
SamplerState rSampler : register(s14);


float cOffset = 0.006f;
float offset = 1.0f / 640.0f;
float4 main(Input input) : SV_TARGET
{

    float pixelSize = 1.0 / 640.0f;
    float pixelSize2 = 1.0 / 360.0f;

    float kernel2 = 3.0;
    float weight = 1.0;

    // Horizontal Blur
    float3 accumulation = float3(0.0f,0.0f,0.0f);
    float3 weightsum = float3(0.0f, 0.0f, 0.0f);
    for (float i = -kernel2; i <= kernel2; i++) {
        float3 accum = renderView.Sample(rSampler, input.uv + float2(i * pixelSize, 0.0)).rgb;

        accumulation += accum * weight;
        weightsum += weight;
    }

    for (float i = -kernel2; i <= kernel2; i++) {
        float3 accum = renderView.Sample(rSampler, input.uv + float2(0.0f, i * pixelSize2)).rgb;

        accumulation += accum * weight;
        weightsum += weight;
    }
    return float4(accumulation / weightsum, 1.0f);
}