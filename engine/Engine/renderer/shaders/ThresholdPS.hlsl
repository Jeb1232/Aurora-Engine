struct Input {
    float4 position : SV_POSITION;
    float2 uv: UV;
};

Texture2D renderView : register(t15);
Texture2D depthTexture : register(t14);
SamplerState rSampler : register(s14);

cbuffer PostProcessCBuffer {
    matrix prevViewMatrix;
    matrix prevProjectionMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    float3 lightPos;
    float3 camPos;
    //float3 padding;
    float padding;
    float padding2;
};


float4 main(Input input) : SV_TARGET
{
    float depth = depthTexture.Sample(rSampler, input.uv).r;
    float4 col = renderView.Sample(rSampler, input.uv);
    
    float brightness = dot(col.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    if (brightness > 1.0f) {
        col.r = 0.0f;
        col.g = 0.0f;
        col.b = 0.0f;
    }
    

    float threshold = 0.05f;

    if (col.r < threshold) {
        col.r = 0.0f;
        col.g = 0.0f;
        col.b = 0.0f;
    }
    if (col.g < threshold) {
        col.r = 0.0f;
        col.g = 0.0f;
        col.b = 0.0f;
    }
    if (col.b < threshold) {
        col.r = 0.0f;
        col.g = 0.0f;
        col.b = 0.0f;
    }

    return col;
}