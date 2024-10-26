struct Input {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 uv: UV;
};

TextureCube skyboxT : register(t20);
SamplerState skySampler : register(s15);

float4 main(Input input) : SV_TARGET
{
    return skyboxT.Sample(skySampler, input.uv);
}