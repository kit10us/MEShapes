struct VS_IN
{
	float3 position	: POSITION;
	float2 uv		: TEXCOORD;
};

struct VS_OUT
{
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD;
};

Texture2D diffuseMap;

SamplerState linearSample
{
};

cbuffer Constants
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

VS_OUT vs_main( in VS_IN vs_in )
{
	VS_OUT vs_out;
	vs_out.position = float4( vs_in.position, 1.0f );
	vs_out.position = mul( vs_out.position, transpose( worldMatrix ) );
	vs_out.position = mul( vs_out.position, transpose( viewMatrix ) );
	vs_out.position = mul( vs_out.position, transpose( projectionMatrix ) );	
	vs_out.uv = vs_in.uv;
	return vs_out;
}

float4 ps_main( in VS_OUT ps_in ) : SV_TARGET
{
	return diffuseMap.Sample( linearSample, ps_in.uv );
}
