struct Input {
	float2 position : POSITION;
	float3 color : COLOR;
};

struct Output {
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

cbuffer CBuf {
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

Output main(Input input) {
	Output output;
	float4 position;

	position = float4(input.position.x, input.position.y, 0, 1);

	output.position = mul(modelMatrix, position);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);
	output.color = input.color;

	return output;
}