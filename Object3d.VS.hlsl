struct vertexShaderOutput
{
	float4 position : SV_POSITION;
};

struct VertexShaderInput
{
	float4 position : POSITION0;
};

vertexShaderOutput main(VertexShaderInput input)
{	
	vertexShaderOutput output;
	output.position = input.position;
	
	return output;	
}