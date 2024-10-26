cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 cameraPosition;
};

struct ConstantOutputType
{
	float edges[3] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv: UV;
};


Texture2D heightMap;
SamplerState samp;

[domain("tri")]

PixelInputType ColorDomainShader(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<HullOutputType, 3> patch)
{
    float3 vertexPosition;
    PixelInputType output;


	float4 height = heightMap.SampleLevel(samp, uvwCoord.x * patch[0].uv + uvwCoord.y * patch[1].uv + uvwCoord.z * patch[2].uv, 0);

    // Determine the position of the new vertex.
    vertexPosition = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;

	vertexPosition += patch[0].normal * height.r * 1;

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(worldMatrix, float4(vertexPosition, 1.0f));
    output.position = mul(viewMatrix, output.position);
    output.position = mul(projectionMatrix, output.position);

	output.normal = uvwCoord.x * patch[0].normal + uvwCoord.y * patch[1].normal + uvwCoord.z * patch[2].normal;

    // Send the input color into the pixel shader.
    output.uv = uvwCoord.x * patch[0].uv + uvwCoord.y * patch[1].uv + uvwCoord.z * patch[2].uv;

    return output;
}
