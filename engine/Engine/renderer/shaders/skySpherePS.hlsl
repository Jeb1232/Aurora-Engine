struct Input {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 uv : UV;
	float3 worldDir : WDIR;
};

cbuffer skySphereCB {
	float3 sunDirection;
	float padding;
};

float4 main(Input input) : SV_TARGET
{
	float3 normal = normalize(input.normal);
	float3 sunDir = normalize(sunDirection);
	float3 dir = normalize(input.worldDir);

	float3 baseSkyColor = float3(0.2078f, 0.3176f, 0.3607f);
	float3 baseHorizonColor = float3(0.75f, 0.75f, 0.75f);
	float3 bottomColor = float3(0.1f, 0.1f, 0.1f);

	float sunHorizonT = 1.0f - saturate(abs(sunDir.y) * 2.5f); // Strongest at sunDir.y = 0
	float3 sunsetColor = float3(1.0f, 0.4f, 0.1f);
	float3 sunsetSkyColor = float3(0.0f, 0.1137f, 0.1490f);

	float3 horizonColor = lerp(baseHorizonColor, sunsetColor, sunHorizonT);
	float3 skyColor = lerp(baseSkyColor, sunsetSkyColor, sunHorizonT);

	float3 baseColor = float3(0.1f, 0.1f, 0.1f);
	float N = 80.0f;
	if (dir.y > 0) {

		float t = saturate(dir.y);
		t = log2(t * N + 1) / log2(N + 1);

		baseColor = lerp(horizonColor, skyColor, t);
	}
	else if (dir.y < 0) {


		float t = saturate(-dir.y);
		t = log2(t * N + 1) / log2(N + 1);

		baseColor = lerp(horizonColor, bottomColor, t);
	}

	//float3 lightDir = normalize(float3(0.0f, 1.0f, 0.0f));
	//float diff = max(dot(normal, lightDir), 0);
	float SharpnessFactor = 1250.0; 
	float d = saturate(dot(dir, -sunDir));

	float sunAmount = exp(-pow((1 - d) * SharpnessFactor, 4.0));

	

	float3 sun = sunAmount * float3(10.0f, 10.0f, 10.0f);

	float3 finalColor = baseColor + sun;
	return float4(finalColor, 1.0f);
}