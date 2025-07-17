struct Input {
    float4 position : SV_POSITION;
    float2 uv: UV;
};

Texture2D renderView : register(t15);
Texture2D depthTexture : register(t14);
SamplerState rSampler : register(s14);

cbuffer bloomCBuffer {
    float3 padding;
    bool horizontal;
    bool pad1;
    bool pad2;
    bool pad3;
};




float4 main(Input input) : SV_TARGET
{
    float depth = depthTexture.Sample(rSampler, input.uv).r;


    float rWidth = 0.0f;
    float rHeight = 0.0f;
    renderView.GetDimensions(rWidth, rHeight);


    float offsetX = 1.0f / rWidth;
    float offsetY = 1.0f / rHeight;
    /*
    float2 offsets[9] =
    {
        float2(-offsetX, offsetY),  //top - left
        float2(0.0f, offsetY),   //top - center
        float2(offsetX, offsetY),  //top - right
        float2(-offsetX, 0.0f),  //center-left
        float2(0.0f, 0.0f),      //center-center
        float2(offsetX, 0.0f),   //center-right
        float2(-offsetX, -offsetY), //bottom - left
        float2(0.0f, -offsetY),  //bottom - center
        float2(offsetX, -offsetY)  //bottom-right
    };

    float kernel[9] =
    {
        1.0f / 16.0f , 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f , 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f , 2.0f / 16.0f, 1.0f / 16.0f
    };
    float3 blurCol = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 9; ++i)
    {
        blurCol += renderView.Sample(rSampler, input.uv + offsets[i]).rgb * kernel[i];;
    }
    */
    float weight[5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };

    float3 result = renderView.Sample(rSampler, input.uv).rgb * weight[0]; // current fragment's contribution
    if (horizontal) {
        for (int i = 1; i < 5; ++i)
        {
            result += renderView.Sample(rSampler, input.uv + float2(offsetX * i, 0.0f)).rgb * weight[i];
            result += renderView.Sample(rSampler, input.uv - float2(offsetX * i, 0.0f)).rgb * weight[i];
        }
    }
    else if (!horizontal) {
        for (int i = 1; i < 5; ++i)
        {
            result += renderView.Sample(rSampler, input.uv + float2(0.0f, offsetY * i)).rgb * weight[i];
            result += renderView.Sample(rSampler, input.uv - float2(0.0f, offsetY * i)).rgb * weight[i];
        }
    }


    return float4(result, 1.0f);
}