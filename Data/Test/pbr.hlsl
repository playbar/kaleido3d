struct PS_IN
{
	float3 Pos : POSITION;
	float3 Nor : NORMAL;
	floaT2 UV  : TEXCOORD;	
};

cbuffer PBSConst : register(b0)
{
	float roughness;
	float metallic;
	float4 baseColor;
	float4 lightColor;
};

const float PI = 3.14159265359;

float D_GGX(float NoH, float roughness)
{
	float A = roughness * roughness;
	float A2 = A * A;
	float d = NoH * NoH * (A2 - 1.0) + 1.0;
	return A2 / (PI * d * d);
}

float G_SchlickmithGGX(float NoL, float NoV, float roughness)
{
	float r = roughness + 1.0;
	float k = r * r / 8.0;
	float GL = NoL / (NoL*(1.0-k) + k);
	float GV = NoV / (NoV*(1.0-k) + k);
	return GL * GV;
}

// fresnel factor
float3 F_Schlick(float cosTheta, float metallic)
{
	float3 F0 = lerp(float3(0.04), baseColor, metallic);
	float3 F = F0 + (1-F0) * pow(1-cosTheta, 5.0);
	return F;
}

float3 BRDF(float3 L, float3 V, float3 N, float metallic, float roughness)
{
	float3 color = float3(0.0);
	float3 H = normalize(V+L);
	//float3 lightColor= float3(1.0);
	float NoV = saturate(dot(N, V));
	float NoL = saturate(dot(N, L));
	float LoH = saturate(dot(L, H));
	float NoH = saturate(dot(N, H));
	if(NoL > 0.0)
	{
		float D = D_GGX(NoH, roughness);
		float G = G_SchlickmethGGX(NoL, NoV, roughness);
		float3 F = F_Schlick(NoV, metallic);
		// Microfacet specular = D*G*F / (4*NoL*NoV)
		float3 specular = D * F * G / (4.0 * NoL * NoV);
		color += specular * NoL * lightColor.xyz;
	}
	return color;
}