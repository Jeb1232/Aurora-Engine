struct Input {
    float4 position : SV_POSITION;
    float2 uv: UV;
};

float density = 0.1f;
float near_plane = 0.1f;
float far_plane = 10000.0f;
Texture2D renderView : register(t15);
Texture2D depthTexture : register(t14);
SamplerState rSampler : register(s14);
Texture2D shadowMap : register(t8);
SamplerState shadowSampler : register(s13);

cbuffer PostProcessCBuffer {
    matrix prevProjectionMatrix;
    matrix projectionMatrix;
    float3 camPos;
};

static const float3x3 ACESInputMat =
{
    {0.59719, 0.35458, 0.04823},
    {0.07600, 0.90834, 0.01566},
    {0.02840, 0.13383, 0.83777}
};

static const float3x3 ACESOutputMat =
{
    { 1.60475, -0.53108, -0.07367},
    {-0.10208,  1.10813, -0.00605},
    {-0.00327, -0.07276,  1.07602}
};

float3 RRTAndODTFit(float3 v) {
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float4 main(Input input) : SV_TARGET
{
    float3 col = renderView.Sample(rSampler, input.uv) * 1.0f;
    float depth = depthTexture.Sample(rSampler, input.uv).r;
    float3 shadowmap = shadowMap.Sample(shadowSampler, input.uv);
    col = mul(ACESInputMat, col);
   
    col = RRTAndODTFit(col);

    float3 Cout = mul(ACESOutputMat, col);
    
    float depthValue = LinearizeDepth(depth) * far_plane;
    float fogFactor = (density / sqrt(log(2))) * max(0.0f, depthValue - 0.0f);
    fogFactor = exp2(-fogFactor * fogFactor);

    //return float4(shadowmap, 1.0f);
    return float4(pow(saturate(Cout), 1.0f / 2.2f), 1.0f);
    //return float4(saturate(Cout), 1.0f);
    //return float4(1.0f,1.0f,1.0f,1.0f);
}