struct Input {
	float2 position : Position;
	float2 uv: UV;
};

struct Output {
	float4 position : SV_POSITION;
	float2 uv : UV;
};

Output main(Input input) {
	Output output;

	output.position = float4(input.position, 0.0f, 1.0f);
	output.uv = input.uv;
	return output;
}