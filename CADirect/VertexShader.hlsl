#include "All.fxh" 

//Vertex Shader

//cbuffer ConstantBuffer : register(b0)
//{
//    matrix WorldViewProjection;
//};

//struct VS_INPUT
//{
//    float3 Pos : POSITION;
//    float3 Normal : NORMAL;
//};

//struct PS_INPUT
//{
//    float4 Pos : SV_POSITION;
//    float3 Normal : NORMAL;
//};

//PS_INPUT main(VS_INPUT input)
//{
//    PS_INPUT output = (PS_INPUT) 0;
//    output.Pos = mul(float4(input.Pos, 1.0f), WorldViewProjection); // Konwersja float3 do float4
//    output.Normal = input.Normal;
//    return output;
//}

//// Vertex Shader
//cbuffer ConstantBuffer : register(b0)
//{
//    matrix WorldViewProjection;
//};

//struct VS_INPUT
//{
//    float4 Pos : POSITION;
//    float3 Normal : NORMAL;
//};

//struct PS_INPUT
//{
//    float4 Pos : SV_POSITION;
//    float3 Normal : NORMAL;
//};

////PS_INPUT VS(VS_INPUT input)
//PS_INPUT main(VS_INPUT input)
//{
//    PS_INPUT output = (PS_INPUT) 0;
//    output.Pos = mul(input.Pos, WorldViewProjection);
//    return output;
//}


//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//    return pos;
//}