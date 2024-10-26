cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 cameraPosition;
};

struct HullInputType
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : UV;
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

float ScaleByDistance(float3 pos1, float3 pos2)
{
	float minValue = 1;
	float maxValue = 16;
	float fdistance = distance(pos1, pos2);
	float maxDistance = 30.0;

	fdistance = clamp(fdistance, 0.0, maxDistance);

	

	float scaledValue = lerp(maxValue, minValue, fdistance / maxDistance);

	return scaledValue;
}

ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	float tessellationAmount = 8;

	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = ScaleByDistance(inputPatch[0].position, cameraPosition);
	output.edges[1] = ScaleByDistance(inputPatch[0].position, cameraPosition);
	output.edges[2] = ScaleByDistance(inputPatch[0].position, cameraPosition);

	// Set the tessellation factor for tessallating inside the triangle.
	output.inside = ScaleByDistance(inputPatch[0].position, cameraPosition);

	return output;
}


[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	HullOutputType output;


	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;

	// Set the input color as the output color.
	output.normal = patch[pointId].normal;

	output.uv = patch[pointId].uv;

	return output;
}
