struct Input {
	float3 position : Position;
	float3 normal : NORMAL;
	float2 uv : UV;
	float3 tangent : TANGENT;
};

struct Output {
	float4 position : SV_POSITION;
	float4 depthPosition : DPOS;
	float2 uv : UV;
};

cbuffer CBuf {
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	//matrix lightSpaceMatrix;
};

Output main(Input input) {
	Output output;
	float4 position;
	float3 worldPos;
	position = float4(input.position, 1.0f);
	worldPos = mul(modelMatrix, position);


	output.position = mul(modelMatrix, position);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);

	output.depthPosition = output.position;

	output.uv = input.uv;
	return output;
}