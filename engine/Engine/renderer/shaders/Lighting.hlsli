

// ----------------------------------------------------------------------------
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float PI = 3.14159265359;
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 PBRdir(float3 albedo, float3 normal, float metallic, float roughness, float ao, float3 direction, float3 camPos, float3 worldPos, float shadow, float3 lightColor, float lightIntensity) 
{
    float PI = 3.14159265359;
    float3 N = normalize(normal);
    float3 V = normalize(camPos - worldPos);

    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(albedo, metallic, F0);

    // reflectance equation
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    // calculate per-light radiance
    //float3 L = normalize(input.camPos - input.worldPos);
    //float3 L = normalize(-direction);
    float3 L = normalize(-direction);
    float3 H = normalize(V + L);
    //float distance = length(input.camPos - input.worldPos);
    //float attenuation = 1.0 / (distance * distance);
    float3 radiance = lightColor;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * saturate(dot(N, V)) * saturate(dot(N, L)) + 0.0001; // + 0.0001 to prevent divide by zero
    float3 specular = numerator / denominator;

    // kS is equal to Fresnel
    float3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = saturate(dot(N, L));
    //float shadow = ShadowCalculation(input.worldPosLightSpace);
    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * (radiance * lightIntensity) * NdotL * (1.0 - shadow);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again


    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).


    float3 lightDir = normalize(float3(0.0f, 1.0f, 0.0f));
    float diff = max(dot(normal, lightDir), 0);

    float3 baseSkyColor = float3(0.2078f, 0.3176f, 0.3607f);
    // float3(0.03f, 0.03f, 0.03f)
    float3 ambient = (float3(0.03f, 0.03f, 0.03f) + (baseSkyColor * 0.5f) * diff) * albedo * ao;

    float3 color = ambient + Lo;

    return color;
}

float3 PBRpoint(float3 albedo, float3 normal, float metallic, float roughness, float ao, float3 position, float3 camPos, float3 worldPos, float shadow, float3 lightColor, float lightIntensity)
{
    float PI = 3.14159265359;
    float3 N = normalize(normal);
    float3 V = normalize(camPos - worldPos);

    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(albedo, metallic, F0);

    // reflectance equation
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    // calculate per-light radiance
    //float3 L = normalize(input.camPos - input.worldPos);
    //float3 L = normalize(-direction);
    float3 L = normalize(position - worldPos);
    float3 H = normalize(V + L);
    float distance = length(position - worldPos);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * saturate(dot(N, V)) * saturate(dot(N, L)) + 0.0001; // + 0.0001 to prevent divide by zero
    float3 specular = numerator / denominator;

    // kS is equal to Fresnel
    float3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = saturate(dot(N, L));
    //float shadow = ShadowCalculation(input.worldPosLightSpace);
    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * (radiance * lightIntensity) * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again


    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;

    float3 color = ambient + Lo;

    return color;
}

float3 PBRspot(float3 albedo, float3 normal, float metallic, float roughness, float ao, float3 position, float3 direction, float cutoff, float outerCutoff, float3 camPos, float3 worldPos, float shadow, float3 lightColor, float lightIntensity)
{
    float PI = 3.14159265359;
    float3 N = normalize(normal);
    float3 V = normalize(camPos - worldPos);

    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(albedo, metallic, F0);

    // reflectance equation
    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    // calculate per-light radiance
    //float3 L = normalize(input.camPos - input.worldPos);
    //float3 L = normalize(-direction);
    float3 L = normalize(position - worldPos);

    float theta = dot(L, normalize(-direction));
    float epsilon = cutoff - outerCutoff;
    float spotIntensity = clamp((theta - outerCutoff) / epsilon, 0.0f, 1.0f);

    float3 H = normalize(V + L);
    float distance = length(position - worldPos);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = lightColor * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * saturate(dot(N, V)) * saturate(dot(N, L)) + 0.0001; // + 0.0001 to prevent divide by zero
    float3 specular = numerator / denominator;

    // kS is equal to Fresnel
    float3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;

    // scale light by NdotL
    float NdotL = saturate(dot(N, L));
    //float shadow = ShadowCalculation(input.worldPosLightSpace);
    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * (radiance * lightIntensity) * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again


    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).

    float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;

    float3 color = ambient + (Lo * spotIntensity);

    return color;
}
