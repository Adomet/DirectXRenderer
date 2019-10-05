struct VS_INPUT
{
	float4 position: POSITION;
	float3 color: COLOR;
};

struct VS_OUTPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
};


VS_OUTPUT vsmain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;
	output.color = input.color;

	return output;
}


struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
};


float4 psmain(PS_INPUT input) : SV_TARGET
{
	return float4(input.color,1.0f);
}