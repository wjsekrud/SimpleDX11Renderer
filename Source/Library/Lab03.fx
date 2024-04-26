float4 VS(float4 pos : POSITION) : SV_POSITION {
	return pos;
}

float4 PS(float4 Pos : SV_POSITION) : SV_Target
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}


cbuffer cbChangeOnCameraMovement : register(b0) {
	matrix View;
	float4 CameraPosition;
};
cbuffer cbProjection : register(b1) {
	matrix Projection;
};
cbuffer cbChangesEveryFrame : register(b2) {
	matrix World;
	float4 OutColor;
};

cbuffer cbLights : register(b3) {
	float4 LightPositions[2];
	float4 LightColors[2];
}

struct VS_INPUT
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 WorldPosition : WORLDPOS;
};


float4 PSSolid(PS_INPUT input) : SV_Target{
	return OutColor;
}

/////////////////////////////////
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	output.Normal = normalize(mul(float4(input.Normal, 0.0f), World).xyz);
	output.WorldPosition = mul(input.Position, World);

	output.TexCoord = input.TexCoord;

	return output;
}

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PS_INPUT input) : SV_Target
{
	float3 ambient = 0;
	float3 viewDirection = normalize(CameraPosition.xyz - input.WorldPosition);
	float4 color = txDiffuse.Sample(samLinear, input.TexCoord);
	float3 diffuse = 0;
	float3 specular = 0;
	float3 spTemp = 0;
	for (uint i = 0; i < 2; i++){
		float3 LightDirection = normalize(input.WorldPosition - LightPositions[i].xyz);
		spTemp = reflect(LightDirection, input.Normal);
		ambient += float3(0.1f, 0.1f, 0.1f);
		diffuse += max(dot(input.Normal, -LightDirection), 0) * LightColors[i];
		if (dot(viewDirection, spTemp)>0 ) {
			specular += pow(dot(spTemp, viewDirection),4.0f) * LightColors[i].rgb;
			if (dot(LightDirection, input.Normal) > 0)
				specular -= pow(dot(spTemp, viewDirection), 4.0f) * LightColors[i].rgb;
		}
	}
	return float4(saturate(diffuse + 2*specular + ambient) * color.rgb, color.a);
}



