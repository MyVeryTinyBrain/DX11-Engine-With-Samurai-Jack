SamplerComparisonState	ShadowSampler
{
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	MinLOD = 0.0f;
	MaxLOD = FLT_MAX;
	MipLODBias = 0.0f;
	MaxAnisotropy = 0;
	ComparisonFunc = LESS_EQUAL;
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
};

struct LightCalculateDesc
{
	half4	Light;
	half4	Volumetric;
};

struct ShadowDesc
{
	half	Value;
	bool	InShadowMap;
};

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2

struct LightDesc
{
	bool		DrawShadow;
	uint		Type;
	uint		DepthSize;
	float		ShadowWhiteness;
	float		Intensity;
	float		Range;					// [0~inf]
	float		Angle;					// [0~90]
	float		ShadowBias;				// [0~1]
	float		Near;
	float		Far;
	float4		Position;
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
	matrix		ViewMatrix[6];
	matrix		ProjectionMatrix[6];
};

struct VolumetricDesc
{
	bool		DrawVolumetric;
	uint		NumSamples;
	float		Intensity;
	float		Power;
};

inline half ComputeDistanceAtten(LightDesc light, float3 worldPosition)
{
	float3 lightToPixel = worldPosition - light.Position.xyz;
	half d = length(lightToPixel);
	half distAtten = max(light.Range - d, 0) / light.Range;
	return distAtten;
}

inline half ComputeAngleAtten(LightDesc light, float3 worldPosition)
{
	float3 lightToPixel = worldPosition - light.Position.xyz;
	half cosAngle = cos(light.Angle * Deg2Rad);
	half oneMinusCosAngle = 1 - cosAngle;
	half angleAtten = (dot(normalize(lightToPixel), light.Direction.xyz) - cosAngle) / oneMinusCosAngle;
	angleAtten = saturate(angleAtten);
	return angleAtten;
}

inline half SampleShadow(texture2D lightDepthMap[6], uint index, half2 texCoord, half depth)
{
	switch (index)
	{
		case 0: return lightDepthMap[0].SampleCmpLevelZero(ShadowSampler, texCoord, depth).r;
		case 1: return lightDepthMap[1].SampleCmpLevelZero(ShadowSampler, texCoord, depth).r;
		case 2: return lightDepthMap[2].SampleCmpLevelZero(ShadowSampler, texCoord, depth).r;
		case 3: return lightDepthMap[3].SampleCmpLevelZero(ShadowSampler, texCoord, depth).r;
		case 4: return lightDepthMap[4].SampleCmpLevelZero(ShadowSampler, texCoord, depth).r;
		case 5: return lightDepthMap[5].SampleCmpLevelZero(ShadowSampler, texCoord, depth).r;
		default: return lightDepthMap[0].SampleCmpLevelZero(ShadowSampler, texCoord, depth).r;
	}
}

inline ShadowDesc ComputeShadowCommon(LightDesc light, texture2D lightDepthMap[6], uint index, half shadowWhiteness, float3 worldPosition)
{
	ShadowDesc shadowDesc = (ShadowDesc)0;

	half4 lightViewPosition;
	lightViewPosition = mul(float4(worldPosition, 1.0f), light.ViewMatrix[index]);
	lightViewPosition = mul(lightViewPosition, light.ProjectionMatrix[index]);

	half2 projectTexCoord;
	projectTexCoord.x = (lightViewPosition.x / lightViewPosition.w) / 2.0f + 0.5f;
	projectTexCoord.y = -(lightViewPosition.y / lightViewPosition.w) / 2.0f + 0.5f;

	half depthValueByCamera = lightViewPosition.z / lightViewPosition.w;
	depthValueByCamera = depthValueByCamera - light.ShadowBias;

	half depth = lightViewPosition.z / lightViewPosition.w;

	[flatten]
	if (IsSaturated(projectTexCoord))
	{
		shadowDesc.InShadowMap = true;

		half depthValueByLight = SampleShadow(lightDepthMap, index, projectTexCoord, depth);

		[flatten]
		if (depthValueByCamera <= depthValueByLight)
		{
			shadowDesc.Value += 1.0f;
		}
		else
		{
			shadowDesc.Value += shadowWhiteness;
		}
	}
	else
	{
		shadowDesc.InShadowMap = false;
		return shadowDesc;
	}

	return shadowDesc;
}

inline ShadowDesc ComputeShadowCommonPCF3X3(LightDesc light, texture2D lightDepthMap[6], uint index, half shadowWhiteness, float3 worldPosition)
{
	ShadowDesc shadowDesc = (ShadowDesc)0;

	half4 lightViewPosition;
	lightViewPosition = mul(float4(worldPosition, 1.0f), light.ViewMatrix[index]);
	lightViewPosition = mul(lightViewPosition, light.ProjectionMatrix[index]);

	half2 projectTexCoord;
	projectTexCoord.x = (lightViewPosition.x / lightViewPosition.w) / 2.0f + 0.5f;
	projectTexCoord.y = -(lightViewPosition.y / lightViewPosition.w) / 2.0f + 0.5f;

	half depthValueByCamera = lightViewPosition.z / lightViewPosition.w;
	depthValueByCamera = depthValueByCamera - light.ShadowBias;

	half depth = lightViewPosition.z / lightViewPosition.w;

	static const int NUM_SAMPLES = 3;
	const half delta = 1.0f / light.DepthSize;
	int numInShadowMap = 1;

	[flatten]
	if (IsSaturated(projectTexCoord))
	{
		shadowDesc.InShadowMap = true;
	}
	else
	{
		shadowDesc.InShadowMap = false;
		return shadowDesc;
	}

	[unroll]
	for (int i = 0; i < NUM_SAMPLES * NUM_SAMPLES; ++i)
	{
		int stepU = (i % NUM_SAMPLES) - (NUM_SAMPLES / 2);
		int stepV = (i / NUM_SAMPLES) - NUM_SAMPLES / 2;
		half2 sampeUV = projectTexCoord + half2(stepU, stepV) * delta;
		half depthValueByLight = SampleShadow(lightDepthMap, index, sampeUV, depth);

		[flatten]
		if (depthValueByCamera <= depthValueByLight)
		{
			shadowDesc.Value += 1.0f;
		}
		else
		{
			shadowDesc.Value += shadowWhiteness;
		}
	}

	shadowDesc.Value /= (NUM_SAMPLES * NUM_SAMPLES);
	return shadowDesc;
}

inline half ComputeShadow_Directional(LightDesc light, texture2D lightDepthMap[6], half shadowWhiteness, float3 worldPosition)
{
	// 가까운 쉐도우맵부터 순회합니다.
	// 픽셀의 월드 위치가 i번째 쉐도우맵에 포함되어 있다면 i번째 쉐도우맵에 대하여 연산합니다.
	[unroll]
	for (uint i = 0; i < 3; ++i)
	{
		ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(light, lightDepthMap, i, shadowWhiteness, worldPosition);

		[flatten]
		if (shadowDesc.InShadowMap)
			return shadowDesc.Value;
	}

	// 이 픽셀이 그림자 맵의 영역 외부에 있다면 조명 연산된 밝기를 사용합니다.
	return 1.0f;
}

inline uint DirectionToCubemapIndex(half3 dir)
{
	half3 absDir = abs(dir);
	uint index = 0;
	[flatten]
	if (absDir.z >= absDir.x && absDir.z >= absDir.y && dir.z >= 0) // +Z Plane
	{
		index = 2;
	}
	else if (absDir.z >= absDir.x && absDir.z >= absDir.y && dir.z < 0) // -Z Plane
	{
		index = 4;
	}
	else if (absDir.y >= absDir.x && dir.y >= 0) // +Y Plane
	{
		index = 0;
	}
	else if (absDir.y >= absDir.x && dir.y < 0) // -Y Plane
	{
		index = 5;
	}
	else if (dir.x >= 0) // +X Plane
	{
		index = 3;
	}
	else // -X Plane
	{
		index = 1;
	}
	return index;
}

inline half ComputeShadow_Point(LightDesc light, texture2D lightDepthMap[6], half3 lightToPixel, half shadowWhiteness, float3 worldPosition)
{
	// PointLight.h에 정의된 뷰 행렬의 순서입니다.
	// const V3	m_arrDirection[6] = { V3::up(),V3::left(),V3::forward(),V3::right(),V3::back(),V3::down(), };

	// 선택된 쉐도우맵 인덱스입니다.
	uint shadowmapIndex = DirectionToCubemapIndex(normalize(lightToPixel));

	//// 쉐도우맵의 방향과 조명이 픽셀까지의 방향을 내적한 값들중 최대값입니다.
	//// 이것은 쉐도우맵의 방향과 가장 적은 각도차이를 나타냅니다.
	//// 또한 가장 적은 각도차이를 나타내는 인덱스를 사용하게 됩니다.
	//half maxDot = -2.0f;
	//half dotResult[6];
	//dotResult[0] = dot(lightToPixel, half3(1, 0, 0));
	//dotResult[1] = dot(lightToPixel, -half3(1, 0, 0));
	//dotResult[2] = dot(lightToPixel, half3(0, 0, 1));
	//dotResult[3] = dot(lightToPixel, -half3(0, 0, 1));
	//dotResult[4] = dot(lightToPixel, half3(0, 1, 0));
	//dotResult[5] = dot(lightToPixel, -half3(0, 1, 0));

	//// 가장 적은 각도차이를 가지는 인덱스를 구합니다.
	//[unroll]
	//for (uint i = 0; i < 6; ++i)
	//{
	//	[flatten]
	//	if (dotResult[i] > maxDot)
	//	{
	//		maxDot = dotResult[i];
	//		shadowmapIndex = i;
	//	}
	//}

	ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(light, lightDepthMap, shadowmapIndex, shadowWhiteness, worldPosition);
	return shadowDesc.Value;
}

inline half ComputeShadow_Spot(LightDesc light, texture2D lightDepthMap[6], half shadowWhiteness, float3 worldPosition)
{
	ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(light, lightDepthMap, 0, shadowWhiteness, worldPosition);
	return shadowDesc.Value;
}

inline int RaycastSphere(float3 rayPoint, half3 rayDir, float3 spherePoint, float radius, out float3 intersects[2])
{
	float3 o_minus_c = rayPoint - spherePoint;

	float p = dot(rayDir, o_minus_c);
	float q = dot(o_minus_c, o_minus_c) - (radius * radius);

	float discriminant = (p * p) - q;

	[flatten]
	if (discriminant < 0.0f)
	{
		return 0.0f;
	}
	else
	{
		float dRoot = sqrt(discriminant);
		float dist0 = -p - abs(dRoot);
		float dist1 = -p + abs(dRoot);

		intersects[0] = rayPoint + rayDir * dist0;
		intersects[1] = rayPoint + rayDir * dist1;

		return (discriminant > 1e-7) ? 2 : 1;
	}
}

inline half4 ComputeVolumetric_Point(LightDesc light, VolumetricDesc volumetricDesc, texture2D lightDepthMap[6], float3 worldPosition, float3 cameraPosition)
{
	float3 viewToPixel = worldPosition - cameraPosition;
	float3 spherePoint = light.Position.xyz;
	float sphereRadius = light.Range;
	float3 intersects[2];
	int numPoints = RaycastSphere(cameraPosition, normalize(viewToPixel), spherePoint, sphereRadius, intersects);

	float3 start, end;

	[flatten]
	if (numPoints == 0)
	{
		return half4(0, 0, 0, 0);
	}
	else if (numPoints == 1 || length(spherePoint - cameraPosition) < sphereRadius)
	{
		start = cameraPosition;
		end = intersects[1];
	}
	else
	{
		start = intersects[0];
		end = intersects[1];
	}

	half useIntersectEndPoint = length(cameraPosition - worldPosition) > length(cameraPosition - end);
	end = lerp(worldPosition, end, useIntersectEndPoint);

	float3 delta = end - start;
	float deltaDistance = length(delta);
	half3 dir = delta / deltaDistance;

	uint numStep = volumetricDesc.NumSamples;
	half step = deltaDistance / numStep;

	half shaft = 0.0f;
	[loop]
	for (uint i = 0; i < numStep; ++i)
	{
		half distance = step * i;
		float3 wp;
		wp = start + dir * distance;

		half3 lightToWP = (wp - light.Position.xyz);
		half da = ComputeDistanceAtten(light, wp);
		half wpShaft = ComputeShadow_Point(light, lightDepthMap, lightToWP, 0, wp) * da;
		shaft += wpShaft;
	}
	shaft /= numStep;
	shaft = saturate(shaft * volumetricDesc.Intensity);
	shaft = pow(shaft, volumetricDesc.Power);
	return half4(light.Diffuse.rgb * shaft, 1.0f);
}

inline half4 ComputeVolumetric_Spot(LightDesc light, VolumetricDesc volumetricDesc, texture2D lightDepthMap[6], float3 worldPosition, float3 cameraPosition)
{
	float3 viewToPixel = worldPosition - cameraPosition;
	float coneSideLength = light.Range / cos(light.Angle * Deg2Rad);
	float3 spherePoint = light.Position.xyz;
	float sphereRadius = coneSideLength;
	float3 intersects[2];
	int numPoints = RaycastSphere(cameraPosition, normalize(viewToPixel), spherePoint, sphereRadius, intersects);

	float3 start, end;

	[flatten]
	if (numPoints == 0)
	{
		return half4(0, 0, 0, 0);
	}
	else if (numPoints == 1 || length(spherePoint - cameraPosition) < sphereRadius)
	{
		start = cameraPosition;
		end = intersects[1];
	}
	else
	{
		start = intersects[0];
		end = intersects[1];
	}

	half useIntersectEndPoint = length(cameraPosition - worldPosition) > length(cameraPosition - end);
	end = lerp(worldPosition, end, useIntersectEndPoint);

	float3 delta = end - start;
	float deltaDistance = length(delta);
	half3 dir = delta / deltaDistance;

	uint numStep = volumetricDesc.NumSamples;
	half step = deltaDistance / numStep;

	half shaft = 0.0f;
	[loop]
	for (uint i = 0; i < numStep; ++i)
	{
		half distance = step * i;
		float3 wp;
		wp = start + dir * distance;

		half da = ComputeDistanceAtten(light, wp);
		half aa = ComputeAngleAtten(light, wp);
		half a = da * aa;
		half wpShaft = ComputeShadow_Spot(light, lightDepthMap, 0, wp) * a;
		shaft += wpShaft;
	}
	shaft /= numStep;
	shaft = saturate(shaft * volumetricDesc.Intensity);
	shaft = pow(shaft, volumetricDesc.Power);
	return half4(light.Diffuse.rgb * shaft, 1.0f);
}

inline float distributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

inline float geometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

inline float geometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

inline float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

inline float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(1.0f - roughness, F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

inline half3 ComputePBRLightIntensity(LightDesc light, half atten, half3 albedo, half3 normal, float3 world2Light, float3 world2Camera, half roughness, half metallic, out half intensity)
{
	roughness = min(roughness, 0.995f);

	float3 N = normal;
	float3 V = normalize(world2Camera);
	float3 R = reflect(-V, N);

	//float3 F0 = lerp(0.04f, pow(albedo, 2.2f), metallic);
	float3 F0 = lerp(0.04f, albedo, metallic);

	float3 L = normalize(world2Light);
	float3 H = normalize(V + L);

	float3 radiance = light.Diffuse.rgb * light.Intensity * atten;

	// Cook-Torrance BRDF
	float NDF = distributionGGX(N, H, roughness);
	float G = geometrySmith(N, V, L, roughness);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

	float3 numerator = NDF * G * F;
	// + 0.0001 to prevent divide by zero
	float specularStrength = 2.0f; // Default = 4.0
	float denominator = specularStrength * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f;
	float3 specular = numerator / denominator;

	float3 kS = F;
	float3 kD = 1.0f - kS;
	kD *= 1.0f - metallic;

	float NdotL = max(dot(N, L), 0.0f);

	intensity = light.Intensity * atten * NdotL;

	return (kD * albedo / PI + specular) * NdotL * radiance;

	//// HDR tonemapping
	//Lo = Lo / (Lo + 1.0f);
	//// Gamma correction
	//Lo = pow(Lo, 1.0f / 2.2f);
}

inline LightCalculateDesc ComputeDirectionalLight(LightDesc light, VolumetricDesc volumetricDesc, texture2D lightDepthMap[6], half3 albedo, half3 normal, float3 worldPosition, half depth, half shadowMask, half roughness, half metallic)
{
	half atten = 1.0f;

	float3 world2Light = -light.Direction.xyz;
	float3 world2Camera = _ViewPosition.xyz - worldPosition;
	half lightIntensity;
	half3 Lo = ComputePBRLightIntensity(light, atten, albedo, normal, world2Light, world2Camera, roughness, metallic, lightIntensity);

	half shadow = 1.0f;
	[branch]
	if (light.DrawShadow && shadowMask > 0.0f)
	{
		shadow = ComputeShadow_Directional(light, lightDepthMap, light.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	Lo *= shadow;

	LightCalculateDesc result;
	result.Light = half4(Lo + light.Ambient.rgb * albedo, 1.0f);
	result.Volumetric = half4(0, 0, 0, 1);

	return result;
}

inline LightCalculateDesc ComputePointLight(LightDesc light, VolumetricDesc volumetricDesc, texture2D lightDepthMap[6], half3 albedo, half3 normal, float3 worldPosition, half depth, half shadowMask, half roughness, half metallic)
{
	half3 lightToPixel = (worldPosition - light.Position.xyz);

	half atten = ComputeDistanceAtten(light, worldPosition);
	float3 world2Light = light.Position.xyz - worldPosition;
	float3 world2Camera = _ViewPosition.xyz - worldPosition;
	half lightIntensity;
	half3 Lo = ComputePBRLightIntensity(light, atten, albedo, normal, world2Light, world2Camera, roughness, metallic, lightIntensity);

	half shadow = 1.0f;
	[branch]
	if (light.DrawShadow && shadowMask > 0.0f)
	{
		shadow = ComputeShadow_Point(light, lightDepthMap, lightToPixel, light.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	Lo *= shadow;

	half4 volumetric = half4(0, 0, 0, 1);
	[branch]
	if (volumetricDesc.DrawVolumetric)
	{
		volumetric = ComputeVolumetric_Point(light, volumetricDesc, lightDepthMap, worldPosition, _ViewPosition.xyz);
	}

	LightCalculateDesc output;
	output.Light = half4(Lo + light.Ambient.rgb * albedo * atten, 1.0f);
	output.Volumetric = volumetric;
	return output;
}

inline LightCalculateDesc ComputeSpotLight(LightDesc light, VolumetricDesc volumetricDesc, texture2D lightDepthMap[6], half3 albedo, half3 normal, float3 worldPosition, half depth, half shadowMask, half roughness, half metallic)
{
	half3 lightToPixel = (worldPosition - light.Position.xyz);

	half distAtten = ComputeDistanceAtten(light, worldPosition);
	half angleAtten = ComputeAngleAtten(light, worldPosition);
	half atten = distAtten * angleAtten;
	float3 world2Light = light.Position.xyz - worldPosition;
	float3 world2Camera = _ViewPosition.xyz - worldPosition;
	half lightIntensity;
	half3 Lo = ComputePBRLightIntensity(light, atten, albedo, normal, world2Light, world2Camera, roughness, metallic, lightIntensity);

	half shadow = 1.0f;
	[branch]
	if (light.DrawShadow && shadowMask > 0.0f)
	{
		shadow = ComputeShadow_Spot(light, lightDepthMap, light.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	Lo *= shadow;

	half4 volumetric = half4(0, 0, 0, 1);
	[branch]
	if (volumetricDesc.DrawVolumetric)
	{
		volumetric = ComputeVolumetric_Spot(light, volumetricDesc, lightDepthMap, worldPosition, _ViewPosition.xyz);
	}

	LightCalculateDesc output;
	output.Light = half4(Lo + light.Ambient.rgb * albedo * atten, 1.0f);
	output.Volumetric = volumetric;
	return output;
}

inline LightCalculateDesc ComputeLight(LightDesc light, VolumetricDesc volumetricDesc, texture2D lightDepthMap[6], half3 albedo, half3 normal, float3 worldPosition, half depth, half shadowMask, half roughness, half metallic)
{
	LightCalculateDesc output = (LightCalculateDesc)0;

	[branch]
	switch (light.Type)
	{
		case LIGHT_TYPE_DIRECTIONAL:
			output = ComputeDirectionalLight(light, volumetricDesc, lightDepthMap, albedo, normal, worldPosition, depth, shadowMask, roughness, metallic);
			break;
		case LIGHT_TYPE_POINT:
			output = ComputePointLight(light, volumetricDesc, lightDepthMap, albedo, normal, worldPosition, depth, shadowMask, roughness, metallic);
			break;
		case LIGHT_TYPE_SPOT:
			output = ComputeSpotLight(light, volumetricDesc, lightDepthMap, albedo, normal, worldPosition, depth, shadowMask, roughness, metallic);
			break;
	}

	return output;
}
