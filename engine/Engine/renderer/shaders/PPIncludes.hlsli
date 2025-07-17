float4 DepthToWorldPos(float depth, float2 uv, matrix viewMat, matrix projMat)
{
    float zOverW = depth; // H is the viewport position at this pixel in the range -1 to 1.

    float4 H = float4(uv, zOverW, 1.0f); // Transform by the view-projection inverse.
    float4 D = mul(projMat, H); // Divide by w to get the world position.
    D = mul(viewMat, D);
    return D / D.w;
}

float4 worldPosToScreenPos(float3 worldPos, matrix viewMat, matrix projMat) {
    float4 ScreenLightPos = mul(viewMat, float4(worldPos, 1.0f));

    ScreenLightPos = mul(projMat, ScreenLightPos);
    ScreenLightPos.x = (ScreenLightPos.x / ScreenLightPos.w + 1) * 0.5f;
    ScreenLightPos.y = 1 - ((ScreenLightPos.y / ScreenLightPos.w + 1) * 0.5f);

    return ScreenLightPos;
}