float4 main() : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

//cos z przyk�adu mi si� nie zgadza, zostawi� jednobarwnie
// Pixel Shader
//float4 PS(PS_INPUT input) : SV_Target
//{
//    return float4(1.0f, 0.0f, 0.0f, 1.0f); // Czerwony kolor
//}