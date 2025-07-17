#include"PPIncludes.hlsli"

struct Input {
    float4 position : SV_POSITION;
    float2 uv: UV;
};

float density = 0.01f;
const float near_plane = 0.01f;
const float far_plane = 10000.0f;
Texture2D renderView : register(t15);
Texture2D occView : register(t7);
Texture2D bloomRenderView : register(t12);
Texture2D depthTexture : register(t14);
SamplerState rSampler : register(s14);
Texture2D shadowMap : register(t8);
SamplerState shadowSampler : register(s13);

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

cbuffer PostProcessCBuffer2 {
    matrix mViewMatrix;
    matrix mProjectionMatrix;
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
    float depth1 = (1 - far_plane / near_plane) * depth + (far_plane / near_plane);
    return 1.0f / depth1;
}

float LinearizeDepth2(float depth)
{
    return near_plane * far_plane / (far_plane + depth * (near_plane - far_plane));
}


float cOffset = 0.006f;
float offsetX = 1.0f / 1280.0f * 0.5f;
float offsetY = 1.0f / 720.0f * 0.5f;
float4 main(Input input) : SV_TARGET
{
    float depth = depthTexture.Sample(rSampler, input.uv).r;
    

    float2 newUV = input.uv;
    

    float3 col = renderView.Sample(rSampler, input.uv) * 1.0f;
    float3 bloomCol = bloomRenderView.Sample(rSampler, input.uv) * 1.0f;
    float Ldepth = LinearizeDepth(depth);

    float3 shadowmap = shadowMap.Sample(shadowSampler, input.uv);

    float2 texCoord = input.uv;

    float rWidth = 0.0f;
    float rHeight = 0.0f;
    renderView.GetDimensions(rWidth, rHeight);

    float zOverW = depthTexture.Sample(rSampler, texCoord).r; // H is the viewport position at this pixel in the range -1 to 1.

    float4 H = float4(texCoord.x, texCoord.y, zOverW, 1.0f); // Transform by the view-projection inverse.
    float4 worldPos = DepthToWorldPos(zOverW, texCoord, viewMatrix, projectionMatrix);

    float4 currentPos = H; // Use the world position, and transform by the previous view-
    // projection matrix.
    matrix prevViewProj = prevViewMatrix * prevProjectionMatrix;
    float4 previousPos = mul(prevViewMatrix, worldPos); // Convert to nonhomogeneous points
    previousPos = mul(prevProjectionMatrix, previousPos);
    // [-1,1] by dividing by w. 
    previousPos /= previousPos.w; // Use this
    // frame's position and last frame's to
    // compute the pixel
    // velocity.
    //col = previousPos;

    float2 velocity = (currentPos - previousPos) / 60.0f;
    //col = float3(velocity, zOverW);

    int numSamples = 16;

    //float2 texCoord2 = input.uv;
    float4 color = renderView.Sample(rSampler, texCoord);
    texCoord += velocity;
    for (int i = 0; i < numSamples; ++i, texCoord += velocity)
    {
        // Sample the color buffer along the velocity vector.
        float4 currentColor = renderView.Sample(rSampler, texCoord);
        // Add the current color to our color sum.
        color += currentColor;
    } // Average all of the samples to get the final blur color.
    float4 finalColor = color / numSamples;

    col = finalColor;

    

    float distanceFromCenter = saturate(length(input.uv - 0.5f));
    float2 direction = input.uv - 0.5f;
    float caIntensity = 0.002f;

    float2 redUV = input.uv + (direction * 0.004f);
    float2 greenUV = input.uv + (direction * 0.005f);
    float2 blueUV = input.uv + (direction * 0.006f);

    //col.r = renderView.Sample(rSampler, redUV).r;
    //col.g = renderView.Sample(rSampler, greenUV).g;
    //col.b = renderView.Sample(rSampler, blueUV).b;


    int numRaySamples = 128;
    float Decay = 1.0f;
    float Weight = 0.0025f;
    float Density = 1.0f;
    float4 lightWorldPos = float4(lightPos, 1.0f);
    //float4 ScreenLightPos = lightWorldPos;
    matrix MVP = mViewMatrix * mProjectionMatrix;
    float4 ScreenLightPos = worldPosToScreenPos(lightPos, mViewMatrix, mProjectionMatrix);
    //ScreenLightPos.x = MVP._m00 * lightWorldPos.x + MVP._m01 * lightWorldPos.y + MVP._m02 * lightWorldPos.z + MVP._m03;
    //ScreenLightPos.y = MVP._m10 * lightWorldPos.x + MVP._m11 * lightWorldPos.y + MVP._m12 * lightWorldPos.z + MVP._m13;
    //ScreenLightPos.z = MVP._m20 * lightWorldPos.x + MVP._m21 * lightWorldPos.y + MVP._m22 * lightWorldPos.z + MVP._m23;
    //ScreenLightPos.w = MVP._m30 * lightWorldPos.x + MVP._m31 * lightWorldPos.y + MVP._m32 * lightWorldPos.z + MVP._m33;

    //ScreenLightPos.w = 1.0f / ScreenLightPos.w;
    //ScreenLightPos.x *= ScreenLightPos.w;
    //ScreenLightPos.y *= ScreenLightPos.w;
    //ScreenLightPos.z *= ScreenLightPos.w;

    //float4 ScreenLightPos = float4(0.5f, 0.7f, 1.0f, 1.0f);
    //float3 ScreenLightPos = float3(0.5f, 0.5f, 0.96f);

    bool godrays = false;
    if (ScreenLightPos.w > 0 && godrays) {

        //ScreenLightPos = float4(float3(float2(ScreenLightPos.x + 1.0f, ScreenLightPos.y + 1.0f) * 0.5f, ScreenLightPos.z), ScreenLightPos.w);
        //ScreenLightPos.x = (ScreenLightPos.x * 0.5f + 0.5f) / 1280;
        //ScreenLightPos.y = (ScreenLightPos.y * 0.5f + 0.5f) / 720;
        //ScreenLightPos.z = (ScreenLightPos.z * 0.5f + 0.5f); // Assuming depth range [0, 1]
        float2 texCoord1 = input.uv;
        // * (ScreenLightPos.z < 0 ? -1 : 1)
        float2 deltaTexCoord = (texCoord1 - ScreenLightPos.xy) * (ScreenLightPos.z < 0 ? -1 : 1);
        // Divide by number of samples and scale by control factor.
        deltaTexCoord *= 1.0f / numRaySamples * Density;
        // Store initial sample.
        float3 godrayColor = occView.Sample(rSampler, texCoord1);
        // Set up illumination decay factor.
        float illuminationDecay = 1.0f;
        // Evaluate summation from Equation 3 NUM_SAMPLES iterations.
        //(ScreenLightPos.z < 0 ? 0 : numRaySamples * ScreenLightPos.z)
        for (int i = 0; i < numRaySamples * ScreenLightPos.z; i++)
        {
            // Step sample location along ray.
            texCoord1 -= deltaTexCoord;
            // Retrieve sample at new location.
            float3 sample1 = occView.Sample(rSampler, texCoord1);
            // Apply sample attenuation scale/decay factors.
            sample1 *= illuminationDecay * Weight;
            // Accumulate combined color.

            godrayColor += sample1;

            // Update exponential decay factor.
            illuminationDecay *= Decay;
            
        }
        godrayColor / numRaySamples;

        col += godrayColor;
    }
    
    //col += bloomCol;

    col = mul(ACESInputMat, col);
   
    col = RRTAndODTFit(col);
    //col += bloomColor;
    float3 Cout = mul(ACESOutputMat, col);
    
    float depthValue = LinearizeDepth(depthTexture.Sample(rSampler, input.uv).r) * far_plane;
    float depthValue2 = LinearizeDepth2(depthTexture.Sample(rSampler, input.uv).r) * far_plane;
    float fogFactor2 = saturate((3.0f - depthTexture.Sample(rSampler, input.uv).r) / (3.0f - 0.0f));
    float fogFactor = (density / sqrt(log(2))) * max(0.0f, depthValue);
    fogFactor = exp2(-fogFactor * fogFactor);

    //float depthTex = LinearizeDepth2(depthTexture.Sample(rSampler, input.uv).r);

    //float fogFactor = pow(-depthValue * density, 2);
    //return float4(shadowmap, 1.0f);
    //return float4(lerp(float3(0.5f, 0.5f, 0.5f), pow(saturate(Cout), 1.0f / 2.2f), fogFactor2), 1.0f);
    //return float4(float3(depthTex, depthTex, depthTex), 1.0f);
    return float4(pow(saturate(Cout * 1.0f), 1.0f / 2.2f), 1.0f);
    //return worldPos;
    //return float4(velocity.x, velocity.y, 0.0f, 1.0f);
    //return float4(pow(saturate(lerp(float3(0.5f,0.5f,0.5f), Cout, saturate(fogFactor))), 1.0f / 2.2f), 1.0f);
    //return float4(saturate(Cout), 1.0f);
    //return float4(1.0f,1.0f,1.0f,1.0f);
}