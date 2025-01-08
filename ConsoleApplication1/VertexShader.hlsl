// Vertex Shader
cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

//PS_INPUT VS(VS_INPUT input)
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = mul(input.Pos, WorldViewProjection);
    return output;
}


//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//    return pos;
//}