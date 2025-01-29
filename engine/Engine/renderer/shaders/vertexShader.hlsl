struct Input {
	float3 position : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
};

struct Output {
	float4 position : SV_POSITION;
	float3 camPos : CPOS;
	float3 worldPos : WPOS;
	float4 worldPosLightSpace : WPLS;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
};

cbuffer CBuf {
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightSpaceMatrix;
	float3 cameraPosition;
};

Texture2D heightMap;
SamplerState samp;

Output main(Input input) {
	Output output;
	float4 position;

	float4 height = heightMap.SampleLevel(samp, input.uv, 0);

	position = float4(input.position, 1.0f);
	//position.xyz += input.normal * height.r * 10;


	output.position = mul(modelMatrix, position);
	output.worldPos = mul(modelMatrix, position);


	output.worldPosLightSpace = mul(lightSpaceMatrix, output.worldPos);
	output.normal = mul((float3x3)modelMatrix, input.normal);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);
	output.camPos = cameraPosition;

	//output.tangent = mul((float3x3)modelMatrix, input.tangent);
	//output.tangent = normalize(output.tangent);

	//output.vPos = output.position.xyz;
	output.uv = input.uv;


	return output;
}