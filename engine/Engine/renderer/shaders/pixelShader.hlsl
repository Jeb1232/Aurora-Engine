struct Input {
    float4 position : SV_POSITION;
    float3 camPos : CPOS;
    float3 worldPos : WPOS;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
};

Texture2D diffuseT;
SamplerState dSampler;
Texture2D specularT;
SamplerState sSampler;

cbuffer Light {
    float4 position;
    float4 direction;
    float4 lightColor;
    float4 ambientColor;
    float3 padding;
    float intensity;
    //float3 camPos;
};

float4 main(Input input) : SV_TARGET
{
    float3 ambientC = float3(ambientColor.x, ambientColor.y, ambientColor.z);
    float3 diffuseC = float3(lightColor.x, lightColor.y, lightColor.z);
    float3 specularC = float3(lightColor.x, lightColor.y, lightColor.z);;
	float4 color = diffuseT.Sample(dSampler, input.uv);
    float4 specT = specularT.Sample(sSampler, input.uv);

	if (color.a < 0.5f) {
		discard;
	}


    float3 ambient = ambientC;
    // diffuse 
    float3 norm = normalize(input.normal);
    //float3 lightDir = normalize(direction - input.position);
    float3 lightDir = normalize(position - input.worldPos);
    float diff = saturate(dot(norm, lightDir));
    float3 diffuse = diffuseC * diff;

    float3 viewDirection = input.camPos - input.worldPos;
    viewDirection = normalize(viewDirection);

    //float3 viewDir = normalize(camPos - input.position.xyz);
    float3 reflectDir = reflect(-lightDir, norm);
    float3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(saturate(dot(halfwayDir, norm)), 128);
    float3 specular = specularC * spec * specT.xyz;
    
    float distance = length(position - input.worldPos);
    float attenuation = 1.0 / (1.0f + 0.35f * distance + 0.44 * (distance * distance));

    float3 LightColor = (ambient + diffuse * attenuation + specular * attenuation);

	return float4(color * (LightColor * intensity), 1.0f);
	//return float4(input.uv.x,input.uv.y,0.0f,1.0f);
}