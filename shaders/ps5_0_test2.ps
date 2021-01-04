/***********************************************************
 LIMITEngine Shader File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ResolveSceneColorSRGB.fsh
 @brief Resolve scene color
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
};

SamplerState defaultSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
Texture2D tex0;
float EVOffset = 12.0;

PS_OUTPUT ps_main(PS_INPUT In)
{
	PS_OUTPUT output = (PS_OUTPUT)0;
	output.color.rgb = tex0.Sample(defaultSampler, In.uv).rgb * pow(2.0, EVOffset);
    output.color.a = 1.0;
	return output;
}