struct VS_IN
{
	float3 position	: POSITION;
	float2 uv		: TEXCOORD;
	float3 normal   : NORMAL;
};

struct VS_OUT
{
	float4 position	: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float3 normal   : TEXCOOORD1;
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

	vs_out.normal = mul( vs_in.normal, (float3x3)transpose( worldMatrix ) );

	return vs_out;
}
