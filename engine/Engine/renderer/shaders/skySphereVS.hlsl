struct Input {
	float3 position : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
};

struct Output {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 uv : UV;
	float3 worldDir : WDIR;
};

cbuffer CBuf {
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightSpaceMatrix;
	float3 cameraPosition;
};

Output main(Input input) {
	Output output;

	float4 position;

	position = float4(input.position, 1.0f);

	output.uv = input.position;
	output.position = mul(modelMatrix, position);
	
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);
	output.normal = mul((float3x3)modelMatrix, input.normal);

	output.worldDir = normalize(input.position);

	return output;
}