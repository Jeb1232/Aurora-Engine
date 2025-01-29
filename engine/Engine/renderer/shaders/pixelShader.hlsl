#include"Lighting.hlsli"

struct Input {
    float4 position : SV_POSITION;
    float3 camPos : CPOS;
    float3 worldPos : WPOS;
    float4 worldPosLightSpace : WPLS;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
    //float3 tangent : TANGENT;
};

Texture2D albedoT : register(t0);
Texture2D roughnessT : register(t1);
Texture2D metallicT : register(t2);
Texture2D normalT : register(t3);
Texture2D aoT : register(t4);
SamplerState Sampler : register(s0);
TextureCube skyboxT : register(t20);
SamplerState skySampler : register(s15);
Texture2D shadowMap : register(t18);
SamplerState shadowSampler : register(s13);

cbuffer Light {
    float4 position;
    float4 direction;
    float4 lightColor;
    float4 ambientColor;
    float3 padding;
    float intensity;
    //float3 camPos;
};

float ShadowCalculation(float4 worldPosLightSpace)
{
    float shadow = 0.0f;
    // perform perspective divide
    float3 projCoords = worldPosLightSpace.xyz / worldPosLightSpace.w;
    if (projCoords.z <= 1.0f) {
        // transform to [0,1] range
        projCoords = projCoords * 0.5f + 0.5f;
        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = shadowMap.Sample(shadowSampler, projCoords.xy).r;
        // get depth of current fragment from light's perspective
        float currentDepth = projCoords.z / worldPosLightSpace.w;
        // check whether current frag pos is in shadow

        float bias = 0.005;
        if (currentDepth - bias > closestDepth) {
            shadow = 1.0f;
        }
    }
    return shadow;
}

float4 main(Input input) : SV_TARGET
{
	float4 albedoTex = albedoT.Sample(Sampler, input.uv);
    float4 normalTex = normalT.Sample(Sampler, input.uv);
    float4 metallicTex = metallicT.Sample(Sampler, input.uv);
    float4 roughnessTex = clamp(roughnessT.Sample(Sampler, input.uv), 0.05f, 1.0f);
    float4 aoTex = aoT.Sample(Sampler, input.uv);
    normalTex = normalize((normalTex * 2.0) - 1.0);
    float3 normalMap;
    //normalMap = (normalTex.x * input.tangent) + (normalTex.y * input.bitangent) + (normalTex.z * input.normal);

    //input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);

    //Create the biTangent
    //float3 biTangent = cross(input.normal, input.tangent);

    //Create the "Texture Space"
    //float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

    //Convert normal from normal map to texture space and store in input.normal
    //normalMap = normalize(mul(texSpace, normalTex));

    float metallic = 0.0f;
    //float roughness = clamp(specularT.Sample(sSampler, input.uv),0.05f,1.0f);
    float ao = 1.0f;

	if (albedoTex.a < 0.5f) {
		discard;
	}

    /*
    float3 ambient = ambientC;
    // diffuse 
    float3 norm = normalize(input.normal);
    //float3 lightDir = normalize(direction - input.position);
    float3 lightDir = normalize(position - input.worldPos);
    //float3 lightDir = normalize(-direction);
    float diff = saturate(dot(norm, lightDir));
    float3 diffuse = diffuseC * diff;

    float3 viewDirection = input.camPos - input.worldPos;
    viewDirection = normalize(viewDirection);

    //float3 viewDir = normalize(camPos - input.position.xyz);
    float3 reflectDir = reflect(-lightDir, norm);
    float3 halfwayDir = normalize(lightDir + viewDirection);
    float spec = pow(saturate(dot(halfwayDir, norm)), 128);
    float3 specular = specularC * spec * (1.0 - specT.xyz);
    
    float distance = length(position - input.worldPos);
    float attenuation = 1.0 / (1.0f + 0.09f * distance +
        0.032f * (distance * distance));

    //float3 LightColor = (diffuse * attenuation + specular * attenuation);
    //float3 finalColor = (LightColor * intensity + ambient);
    float3 phongColor = diffuse + specular;
    //return float4(color * pow(finalColor, 1.0 / 2.2), 1.0f);
    */
     
    float3 color = PBRdir(albedoTex, input.normal, metallic, roughnessTex, ao, float3(0.0f, -1.0f, 0.5f), input.camPos, input.worldPos);
    //color = color / (color + float3(1.0f, 1.0f, 1.0f));
    float shadow = ShadowCalculation(input.worldPosLightSpace);
    //reflectColor * (1.0 - specT)
    //float3 LightColor = (ambient + ((diffuse + specular) * attenuation) + reflectColor * (1.0 - specT));
    //return float4(color * (LightColor * intensity), 1.0f);
    return float4(color * (1.0 - shadow), 1.0f);
    //return float4(1.0f, 0.0f, 1.0f, 1.0f);
    //return float4(reflectColor, 1.0f);
	//return float4(input.uv.x,input.uv.y,0.0f,1.0f);
}