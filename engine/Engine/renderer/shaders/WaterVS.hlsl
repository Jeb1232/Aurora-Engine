struct Input {
	float3 position : Position;
	float3 normal : NORMAL;
	float2 uv : UV;
	float3 tangent : TANGENT;
	//float3 biTangent : BITANGENT;
};

struct Output {
	float4 position : SV_POSITION;
	float3 camPos : CPOS;
	float3 worldPos : WPOS;
	float3 camPosTS : CPOSTS;
	float3 worldPosTS : WPOSTS;
	float4 worldPosLightSpace : WPOSL;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float2 uv : UV;
	float3 tangent : TANGENT;
	float3 biTangent : BITANGENT;
	float3x3 TBN : TBNMAT;
};

cbuffer CBuf {
	// NOTE: variables with the same type MUST all be grouped together in a row. variables with different types MUST NOT be in those groups. They MUST be in their own rows.
	// If they are not in their own groups it causes byte alignment issues and corrupts the variable data.
	matrix modelMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	//matrix lightSpaceMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	float3 cameraPosition;
	float padding;
};

Texture2D heightMap;
SamplerState samp;

Output main(Input input) {
	Output output;
	float4 position;

	float4 height = heightMap.SampleLevel(samp, input.uv, 0);
	position = float4(input.position, 1.0f);
	//position.xyz += input.normal * height.r * 10;
	float3 wPos = mul(modelMatrix, position);




	float3 T = normalize(mul((float3x3)modelMatrix, input.tangent));
	float3 N = normalize(mul((float3x3)modelMatrix, input.normal));
	T = normalize(T - dot(T, N) * N);
	float3 biTangent = cross(N, T);
	float3 B = cross(N, T);
	//float3 B = normalize(mul((float3x3)modelMatrix, input.biTangent));


	float3x3 TBN2 = float3x3(input.tangent, biTangent, input.normal);

	//Create the "Texture Space"
	// 
	float3x3 TBN = float3x3(T, B, N);
	float3x3 TBN_inv = transpose(TBN);

	output.TBN = TBN;

	output.camPosTS = mul(TBN_inv, cameraPosition);
	output.worldPosTS = mul(TBN_inv, mul(modelMatrix, position));

	output.position = mul(modelMatrix, position);
	output.worldPos = mul(modelMatrix, position);


	output.worldPosLightSpace = mul(modelMatrix, position);
	output.worldPosLightSpace = mul(lightViewMatrix, output.worldPosLightSpace);
	output.worldPosLightSpace = mul(lightProjectionMatrix, output.worldPosLightSpace);

	//output.worldPosLightSpace = mul(lightSpaceMatrix, output.worldPos);
	output.normal = mul((float3x3)modelMatrix, input.normal);
	output.position = mul(viewMatrix, output.position);
	output.position = mul(projectionMatrix, output.position);
	output.camPos = cameraPosition;

	output.tangent = mul((float3x3)modelMatrix, input.tangent);
	output.tangent = normalize(output.tangent);

	output.biTangent = mul((float3x3)modelMatrix, biTangent);
	output.biTangent = normalize(output.biTangent);

	//output.vPos = output.position.xyz;
	output.uv = input.uv;


	return output;
}