struct Input {
	float3 position : Position;
	float3 normal : NORMAL;
	float2 uv : UV;
	float3 tangent : TANGENT;
};

struct Output {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
	float2 renUV : RUV;
};

cbuffer cBuf {
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Output main(Input input) {
	Output output;

	float4 position = float4(input.position, 1.0f);

	output.position = mul(modelMatrix, position);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);

	output.normal = mul((float3x3)modelMatrix, input.normal);

	output.uv = input.uv;

	float4 posRUV = output.position;

	//posRUV /= posRUV.w;

	float2 screenUV = posRUV.xy;
	screenUV.x = (screenUV.x / posRUV.w + 1) * 0.5f;
	screenUV.y = 1 - ((screenUV.y / posRUV.w + 1) * 0.5f);

	output.renUV = posRUV.xy;

	return output;
}