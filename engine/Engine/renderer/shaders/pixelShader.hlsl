#include"Lighting.hlsli"

struct Input {
    float4 position : SV_POSITION;
    float3 camPos : CPOS;
    float3 worldPos : WPOS;
    float3 camPosTS : CPOSTS;
    float3 worldPosTS : WPOSTS;
    float4 worldPosLightSpace : WPOSL;
	//float3 vPos : Position;
	float3 normal : NORMAL;
	float2 uv: UV;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
    float3x3 TBN : TBNMAT;
    bool isFrontFace : SV_IsFrontFace;
    //float3 tangent : TANGENT;
};

Texture2D albedoT : register(t0);
Texture2D roughnessT : register(t1);
Texture2D metallicT : register(t2);
Texture2D normalT : register(t3);
Texture2D aoT : register(t4);
Texture2D heightT : register(t5);
SamplerState Sampler : register(s0);
TextureCube skyboxT : register(t20);
SamplerState skySampler : register(s15);
Texture2D shadowMap : register(t18);
SamplerState shadowSampler : register(s13);

cbuffer LightBuf {
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
    //float2 shadowTexCoords;
   // shadowTexCoords.x = (worldPosLightSpace.x / worldPosLightSpace.w);
   // shadowTexCoords.y = (worldPosLightSpace.y / worldPosLightSpace.w);
    //float currentDepth = worldPosLightSpace.z / worldPosLightSpace.w;

    // transform to [0,1] range
    //projCoords = projCoords * 0.5f + 0.5f;
    projCoords.x = worldPosLightSpace.x * 0.5f + 0.5f;
    projCoords.y = -worldPosLightSpace.y * 0.5f + 0.5f;
    if (saturate(projCoords).x == projCoords.x && saturate(projCoords).y == projCoords.y) {

        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        //shadowTexCoords
        float closestDepth = shadowMap.Sample(shadowSampler, projCoords.xy).r;

        float currentDepth = projCoords.z;

        //float closestDepth = shadowMap.Sample(shadowSampler, shadowTexCoords).r;
        // get depth of current fragment from light's perspective
        //float currentDepth = projCoords.z;
        // check whether current frag pos is in shadow
        float bias = 0.0002;
        //shadow = shadowMap.SampleCmpLevelZero(shadowSampler, shadowTexCoords, pixelDepth + bias).r;
        if (currentDepth - bias > closestDepth) {
            shadow = 1.0f;
        }
    }
    return shadow;
}
//heightT.Sample(Sampler, texCoords).r;

/*
float2 ParallaxMapping(float2 texCoords, float3 viewDir, float heightScale)
{
    // number of depth layers
    const int minLayers = 8;
    const int maxLayers = 32;
    float numLayers = lerp(maxLayers, minLayers, max(dot(float3(0.0, 0.0, 1.0), 0.0f), viewDir));
    //float numLayers = 10;
    // calculate the size of each layer
    float layerDepth = 1.0f / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0f;
    // the amount to shift the texture coordinates per layer (from vector P)
    float2 P = viewDir.xy / viewDir.z * heightScale;
    float2 deltaTexCoords = P / numLayers;

    // get initial values
    float2  currentTexCoords = texCoords;
    float currentDepthMapValue = 0.0f;
    currentDepthMapValue = heightT.Sample(Sampler, currentTexCoords).r;

    for (int i = 0; i < numLayers; i++)
    {
        if (currentLayerDepth < currentDepthMapValue) {
            // shift texture coordinates along direction of P
            currentTexCoords -= deltaTexCoords;
            // get depthmap value at current texture coordinates
            currentDepthMapValue = heightT.Sample(Sampler, currentTexCoords).r;
            // get depth of next layer
            currentLayerDepth += layerDepth;
        }
    }
    
    // get texture coordinates before collision (reverse operations)
    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = heightT.Sample(Sampler, prevTexCoords).r; -currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
    
    return finalTexCoords;
}
*/

float2 ParallaxMapping(float2 texCoords, float3 viewDir, float height_scale)
{
    float height = heightT.Sample(Sampler, texCoords).r;
    float2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return texCoords - p;
}

float4 main(Input input) : SV_TARGET
{
    float3 I2 = normalize(input.worldPosTS - input.camPosTS);

    float2 uvCoords = input.uv;
    //uvCoords = ParallaxMapping(input.uv, I2, 0.1f);


	float4 albedoTex = albedoT.Sample(Sampler, uvCoords);
    float4 normalTex = normalT.Sample(Sampler, uvCoords);
    float4 metallicTex = metallicT.Sample(Sampler, uvCoords);
    float4 roughnessTex = clamp(roughnessT.Sample(Sampler, uvCoords), 0.05f, 1.0f);
    float4 aoTex = aoT.Sample(Sampler, uvCoords);
    normalTex.rgb = normalize(normalTex.rgb * 2.0f - 1.0f);
    //normalTex = normalize(float4(mul(input.TBN, normalTex.xyz), 1.0f));
    //normalTex = float4((normalTex.x * input.tangent) + (normalTex.y * input.biTangent) + (normalTex.z * input.normal), 1.0f);
    //normalize(mul(input.TBN, normalTex.xyz))
    float3 tbnNorm = mul(input.TBN, normalTex.xyz);
    float3 tangentSpaceNormal = normalize(tbnNorm);

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
    
	if (albedoTex.a < 0.1f) {
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
    float3 I = normalize(input.worldPos - input.camPos);
    float3 R = reflect(I, normalize(input.normal));
    float3 reflectColor = skyboxT.Sample(skySampler, R);
    float3 color;
    //color += PBRpoint(albedoTex, input.normal, metallic, roughnessTex, ao, float3(0.0f, 3.0f, 0.0f), input.camPos, input.worldPos, 0.0f);
    float shadow = ShadowCalculation(input.worldPosLightSpace);
    //tangentSpaceNormal
    if (!input.isFrontFace) {
        input.normal = -input.normal;
        tangentSpaceNormal = -tangentSpaceNormal;
    }
    
   
    
    //color = normalize(input.normal);
    //color = tangentSpaceNormal;
    //color = normalize((normalTex.rgb));/
    //float3(tangentSpaceNormal.x, tangentSpaceNormal.y, tangentSpaceNormal.z)
    color = PBRdir(albedoTex, input.normal, metallic, roughnessTex, ao, direction.xyz, input.camPos, input.worldPos, shadow, lightColor, intensity);
    //color = PBRspot(albedoTex, input.normal, metallic, roughnessTex, ao, position.xyz, direction.xyz, cos(radians(20.0f)), cos(radians(30.0f)), input.camPos, input.worldPos, 0.0f, lightColor, intensity);
    /*
    
    if (input.isFrontFace) {
        color = PBRdir(albedoTex, input.normal, metallic, roughnessTex, ao, direction.xyz, input.camPos, input.worldPos, shadow, lightColor, intensity);
    }
    else if (!input.isFrontFace) {
        color = PBRdir(albedoTex, -input.normal, metallic, roughnessTex, ao, direction.xyz, input.camPos, input.worldPos, shadow, lightColor, intensity);
    }

    */


    //color += PBRpoint(albedoTex, input.normal, metallic, roughnessTex, ao, input.camPos, input.camPos, input.worldPos, 0.0f, lightColor, intensity);
    //color = color / (color + float3(1.0f, 1.0f, 1.0f));
    //reflectColor * (1.0 - specT)
    //float3 LightColor = (ambient + ((diffuse + specular) * attenuation) + reflectColor * (1.0 - specT));
    //return float4(color * (LightColor * intensity), 1.0f);
    bool translucent = true;
    if (!input.isFrontFace && translucent) {
        float3 vLight = normalize(direction.xyz);

        float fEdotL = saturate(dot(I, -vLight));
        float fPowEdotL = fEdotL * fEdotL;
        fPowEdotL *= fPowEdotL;

        // Back diffuse shading, wrapped slightly
        float fLdotNBack = saturate(dot(input.normal, vLight));
        float3 vBackShading = fLdotNBack * fPowEdotL;

        color = float4((vBackShading * albedoTex.rgb) * (float3(0.1f, 0.1f, 0.1f) + (1.0f - shadow)), 1.0f);
    }
    
    return float4(color, 1.0f);

    //float fogFactor2 = saturate((30.0f - input.viewDepth) / (30.0f - 0.0f));


    //return float4(lerp(float3(0.5f,0.5f,0.5f), color, input.viewDepth), 1.0f);
    //return float4(input.viewDepth, input.viewDepth, input.viewDepth, 1.0f);
    
    //return float4(1.0f, 0.0f, 1.0f, 1.0f);
    //return float4(reflectColor, 1.0f);
	//return float4(input.uv.x,input.uv.y,0.0f,1.0f);
}