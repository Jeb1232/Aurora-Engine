struct Input {
    float4 position : SV_POSITION;
    float2 uv: UV;
};

float density = 0.2f;
float near_plane = 0.01f;
float far_plane = 10000.0f;
Texture2D renderView : register(t15);
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
    float padding;
    float padding2;
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

float cOffset = 0.006f;
float offsetX = 1.0f / 1280.0f * 0.5f;
float offsetY = 1.0f / 720.0f * 0.5f;
float4 main(Input input) : SV_TARGET
{
    float3 col = renderView.Sample(rSampler, input.uv) * 1.0f;
    float depth = depthTexture.Sample(rSampler, input.uv).r;
    float Ldepth = LinearizeDepth(depth);

    float3 shadowmap = shadowMap.Sample(shadowSampler, input.uv);

    float2 texCoord = input.uv;

    float zOverW = depthTexture.Sample(rSampler, texCoord).r; // H is the viewport position at this pixel in the range -1 to 1.

    float4 H = float4(texCoord.x * 2 - 1, (1 - texCoord.y) * 2 - 1, zOverW, 1); // Transform by the view-projection inverse.
    matrix invViewProj = viewMatrix * projectionMatrix;
    float4 D = mul(invViewProj, H); // Divide by w to get the world position.
    //D = mul(projectionMatrix, D);
    float4 worldPos = D / D.w;

    float4 currentPos = H; // Use the world position, and transform by the previous view-
    // projection matrix.
    matrix prevViewProj = prevViewMatrix * prevProjectionMatrix;
    float4 previousPos = mul(prevViewMatrix, worldPos); // Convert to nonhomogeneous points
    previousPos = mul(prevProjectionMatrix, previousPos);
    // [-1,1] by dividing by w. 
    // previousPos /= previousPos.w; // Use this
    // frame's position and last frame's to
    // compute the pixel
    // velocity.
    float2 velocity = (currentPos - previousPos) / 2.0f;

    int numSamples = 4;

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

    float distanceFromCenter = saturate(length(input.uv - 0.5f));
    float2 direction = input.uv - 0.5f;
    float caIntensity = 0.002f;

    float2 redUV = input.uv + (direction * 0.001f);
    float2 greenUV = input.uv + (direction * -0.002f);
    float2 blueUV = input.uv + (direction * -0.003f);



    float pixelSize = 1.0 / 640.0f;
    float pixelSize2 = 1.0 / 360.0f;

    float kernel2 = 5.0;
    float weight = 1.0;

    float Pi = 6.28318530718; // Pi*2

    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 8.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}

    float2 Radius = Size / float2(1280, 720);

    // Pixel colour
    float4 Color = renderView.Sample(rSampler, input.uv);

    // Blur calculations
    for (float d = 0.0; d < Pi; d += Pi / Directions)
    {
        for (float i = 1.0 / Quality; i <= 1.0; i += 1.0 / Quality)
        {
            Color += renderView.Sample(rSampler, input.uv + float2(cos(d), sin(d)) * Radius * i);
        }
    }


    // Output to screen
    Color /= Quality * Directions - 15.0;
    

    float3 bloomColor = Color;
    float threshold = 0.5f;
    if (bloomColor.r < threshold) {
       //bloomColor.r = 0.0f;
       //bloomColor.g = 0.0f;
       //bloomColor.b = 0.0f;
    }
    if (bloomColor.g < threshold) {
        //bloomColor.r = 0.0f;
        //bloomColor.g = 0.0f;
        //bloomColor.b = 0.0f;
    }
    if (bloomColor.b < threshold) {
        //bloomColor.r = 0.0f;
        //bloomColor.g = 0.0f;
        //bloomColor.b = 0.0f;
    }
    //bloomColor = mul(ACESInputMat, bloomColor);
    //bloomColor = RRTAndODTFit(bloomColor);
    //mul(ACESOutputMat, bloomColor)
    //return float4(glowColor.rgb, 1.0f);
    //return float4(mul(ACESOutputMat, bloomColor), 1.0f);
    //col += bloomColor;

    int numRaySamples = 64;
    float Decay = 1.0f;
    float Weight = 0.1f;
    float Density = 1.0f;
    float4 lightWorldPos = float4(lightPos, 1.0f);
    float4 ScreenLightPos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    ScreenLightPos = mul(viewMatrix, ScreenLightPos);
    //float4 ScreenLightPos = float4(0.5f, 0.7f, 1.0f, 1.0f);
    //float3 ScreenLightPos = float3(0.5f, 0.5f, 0.96f);
    ScreenLightPos = mul(projectionMatrix, ScreenLightPos);
    //ScreenLightPos /= ScreenLightPos.w;
    ScreenLightPos = float4(ScreenLightPos.x + 1.0f, ScreenLightPos.y + 1.0f, ScreenLightPos.z + 1.0f, ScreenLightPos.w) / 2.0f;
    float2 texCoord1 = input.uv;
    float2 deltaTexCoord = (texCoord1 - ScreenLightPos.xy) * (ScreenLightPos.z < 0 ? -1 : 1);
    // Divide by number of samples and scale by control factor.
    deltaTexCoord *= 1.0f / numRaySamples * Density;
    // Store initial sample.
    float3 godrayColor = renderView.Sample(rSampler, texCoord1);
    // Set up illumination decay factor.
    float illuminationDecay = 1.0f;
    // Evaluate summation from Equation 3 NUM_SAMPLES iterations.
    for (int i = 0; i < (ScreenLightPos.z < 0 ? 0 : numRaySamples * ScreenLightPos.z); i++)
    {
        // Step sample location along ray.
        texCoord1 -= deltaTexCoord;
        // Retrieve sample at new location.
        float3 sample1 = renderView.Sample(rSampler, texCoord1);
        // Apply sample attenuation scale/decay factors.
        sample1 *= illuminationDecay * Weight;
        // Accumulate combined color.
        godrayColor += sample1;
        // Update exponential decay factor.
        illuminationDecay *= Decay;
    }

    //col += godrayColor;

    col = mul(ACESInputMat, col);
   
    col = RRTAndODTFit(col);
    //col += bloomColor;
    float3 Cout = mul(ACESOutputMat, col);
    
    float depthValue = LinearizeDepth(depthTexture.Sample(rSampler, input.uv).r) * far_plane;
    float fogFactor = (density / sqrt(log(2))) * max(0.0f, depthValue);
    fogFactor = exp2(-fogFactor * fogFactor);
    //float fogFactor = pow(-depthValue * density, 2);
    //return float4(shadowmap, 1.0f);
    //return float4(lerp(float3(0.5f, 0.5f, 0.5f), pow(saturate(Cout), 1.0f / 2.2f), saturate(fogFactor)), 1.0f);
    //return float4(float3(depthValue, depthValue, depthValue), 1.0f);
    return float4(pow(saturate(Cout * 1.0f), 1.0f / 2.2f), 1.0f);
    //return worldPos;
    //return float4(velocity.x, velocity.y, 0.0f, 1.0f);
    //return float4(pow(saturate(lerp(float3(0.5f,0.5f,0.5f), Cout, saturate(fogFactor))), 1.0f / 2.2f), 1.0f);
    //return float4(saturate(Cout), 1.0f);
    //return float4(1.0f,1.0f,1.0f,1.0f);
}