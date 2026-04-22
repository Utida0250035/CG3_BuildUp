struct TransformationMatrix
{
	float4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformMatrix : register(b0);

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
	
	output.position = mul(input.position, gTransformMatrix.WVP);
	
	return output;	
}