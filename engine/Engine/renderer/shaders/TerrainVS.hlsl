struct Input {
	float3 position : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
};

struct Output {
	float4 position : SV_POSITION;
	float3 camPos : CPOS;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
};

cbuffer CBuf {
	// NOTE: variables with the same type MUST all be grouped together in a row. variables with different types MUST NOT be in those groups. They MUST be in their own rows.
	// If they are not in their own groups it causes byte alignment issues and corrupts the variable data.
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 cameraPosition;
	float padding;
};

//Texture2D heightMap;
//SamplerState samp;

Output main(Input input) {
	Output output;
	float4 position;

	//float4 height = heightMap.Sample(samp, input.uv);
	position = float4(input.position, 1.0f);
	//position.xyz += input.normal * height.r * 10;


	output.position = mul(modelMatrix, position);

	output.normal = mul((float3x3)modelMatrix, input.normal);

	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);

	output.uv = input.uv;
	output.camPos = cameraPosition;

	return output;
}